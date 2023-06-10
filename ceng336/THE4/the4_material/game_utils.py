import copy
import random
import string
import threading
import json
import logging

with open("cookingSimSettings.json","r") as f:
    SETTINGS = json.loads(f.read())

INGREDIENT_LETTERS = ["M", "P", "B"]
COOKING_LETTERS = ["C", "S"]


def set_logging_level(level_string):
    level = getattr(logging, level_string.upper(), None)
    if level is None:
        raise ValueError(f"Invalid logging level: {level_string}")

    # Set the logging level
    logging.getLogger().setLevel(level)

def sanitize_bytewise(intt, num_bytes):
    byte_list = [byte for byte in int.to_bytes(intt, num_bytes, "big")]
    for i, byte in enumerate(byte_list):
        if chr(byte) in ["$",":"]:
            byte_list[i] = byte+1
    return int.from_bytes(bytes(byte_list), "big")

def check_byte_sanity(intt, num_bytes):
    byte_list = [byte for byte in int.to_bytes(intt, num_bytes, "big")]
    return all([chr(byte) not in ["$",":"] for byte in byte_list])

byte64 = (2**64)-1
def hash_djb2(s):
    hash = 5381
    for x in s:
        hash = (((hash << 5) + hash) + ord(x)) & byte64
    return hash

def hash_336(s):
    for i in range(200):
        s = hash_djb2(s)
        s = f"{s:016x}"
    return s

def hashbytes_sanity_check(bytess):
    return all([chr(byte) not in ["$",":"] for byte in bytess])

# Example usage
set_logging_level(SETTINGS["LOG_LEVEL"])

class InvalidCommandException(Exception):
    def __init__(self, msg):
        self.message = msg
        super().__init__(msg)

class IllformedCommandException(InvalidCommandException):
    def __init__(self, cmd):
        super().__init__(f"Ill-formed command: {cmd}")

class CookCommandException(InvalidCommandException):
    def __init__(self, customer_id, ingredient_indices, cooking_state, msg=""):
        msg = f"Invalid cook command arguments - customer_id: {customer_id} "\
                       f"ingredient indices: {ingredient_indices} - state: {cooking_state}"+msg
        super().__init__(msg)

class SlowCookCommandException(InvalidCommandException):
    def __init__(self, customer_id, ingredient_index, cooking_state, msg=""):
        msg = f"Invalid slow-cook command arguments - customer_id: {customer_id} "\
                       f"ingredient index: {ingredient_index} - state: {cooking_state}"+msg
        super().__init__(msg)

class TossCommandException(InvalidCommandException):
    def __init__(self, ingredient_index, cooking_state, msg=""):
        msg = f"Invalid toss command argument - ingredient index: {ingredient_index} - state: {cooking_state}"+msg
        super().__init__(msg)

class HashReplyCommandException(InvalidCommandException):
    def __init__(self, sent_bytestr, answer_bytestr, msg=""):
        msg = f"Incorrect hash reply {sent_bytestr}, should have been {answer_bytestr}"+msg
        super().__init__(msg)


class Ingredient:
    def __init__(self, letter, cooking=None):
        self.letter = letter
        self.cooking = cooking

    def get_info_bytes(self):
        return bytes(str(self), "ascii")
    
    def get_info_str(self):
        return str(self)

    def __str__(self):
        if self.cooking is None:
            return self.letter
        else:
            return self.cooking
    
    def __repr__(self):
        return str(self)
    
    def __hash__(self):
        l = self.letter if self.letter else "_"
        c = self.cooking if self.cooking else "_"
        return hash(l+c)
    
    def __eq__(self, other):
        if other is None:
            return False
        return self.letter == other.letter and self.cooking == other.cooking

    @staticmethod
    def get_empty_info_str():
        return "N"
    
    @staticmethod
    def get_empty_info_bytes():
        return bytes(Ingredient.get_empty_info_str(), "ascii")

    @staticmethod
    def decode_info_bytes(info_bytes):
        info_str = bytes.decode(info_bytes, "ascii")
        return Ingredient.decode_info_str(info_str)
    
    @staticmethod
    def decode_info_str(info_str):
        if info_str in INGREDIENT_LETTERS:
            return Ingredient(info_str)
        if info_str == "C":
            return Ingredient(None, cooking="C")
        if info_str == "S":
            return Ingredient(None, cooking="S")
        if info_str == "N":
            return None
        raise Exception(f"invalid info str for Ingredient: {info_str}")


    @staticmethod
    def random_ingredient():
        return Ingredient(random.choice(INGREDIENT_LETTERS))

class IngredientContainer:
    def __init__(self, capacity, initial_ingredient_list=[]):
        self.ingredients = [None for _ in range(capacity)]
        self.capacity = capacity
        for ingredient in initial_ingredient_list:
            self.add_ingredient(ingredient)
    
    def get_list(self, remove_empty_slots=True):
        if remove_empty_slots:
            return [ing for ing in self.ingredients if ing is not None]
        else:
            return self.ingredients
    
    def add_ingredient(self, ingredient):
        for i, ing in enumerate(self.ingredients):
            if ing is None:
                self.ingredients[i] = ingredient
                return i
        return -1
    
    def get_ingredient(self, index, raise_index_error_on_empty=True):
        if raise_index_error_on_empty and self.ingredients[index] is None:
            raise IndexError(f"ingredient index {index} is empty")
        return self.ingredients[index]
    
    def get_info_str(self):
        return "".join(Ingredient.get_empty_info_str() if ing is None else ing.get_info_str() 
                       for ing in self.ingredients)
    
    def get_info_bytes(self):
        return bytes(self.get_info_str(), "ascii")
    
    def get_empty_info_str(self):
        return "".join(Ingredient.get_empty_info_str() for _ in range(self.capacity))
    
    def get_empty_info_bytes(self):
        return bytes(self.get_empty_info_str(), "ascii")
    
    @staticmethod
    def decode_info_str(info_str):
        ingredients = [Ingredient.decode_info_str(letter) for letter in info_str]
        result = IngredientContainer(len(ingredients))
        result.ingredients = ingredients
        return result
    
    @staticmethod
    def decode_info_bytes(info_bytes):
        return IngredientContainer.decode_info_str(bytes.decode(info_bytes, "ascii"))
    
    def remove_ingredient(self, index, raise_index_error_on_empty=True):
        if raise_index_error_on_empty and self.ingredients[index] is None:
            raise IndexError(f"ingredient index {index} is empty")
        self.ingredients[index] = None
    
    def __str__(self):
        return self.get_info_str()


_customer_cid_counter = 0
class Customer:
    def __init__(self, initial_patience, order_ingredient_list, cid=None):
        global _customer_cid_counter
        if cid is None:
            self.cid = sanitize_bytewise(_customer_cid_counter+1, 1)
            _customer_cid_counter = self.cid
            assert(check_byte_sanity(_customer_cid_counter, 1))
        else:
            self.cid = cid
        self.order_ingredient_container = IngredientContainer(2, order_ingredient_list)
        self.patience = initial_patience
        self.initial_patience = initial_patience

        self.food_judge = False
    
    @staticmethod
    def create_food_judge(cid=None):
        food_judge = Customer(90, [], cid=cid)
        food_judge.food_judge = True
        return food_judge
    
    def get_info_str(self):
        if not self.food_judge:
            return chr(self.cid)+self.order_ingredient_container.get_info_str()+chr(self.get_short_patience())
        else:
            return chr(self.cid)+"FJ"+"\x09"
    
    def get_info_bytes(self):
        if not self.food_judge:
            return int.to_bytes(self.cid, 1, "big")+self.order_ingredient_container.get_info_bytes()+int.to_bytes(self.get_short_patience(), 1, "big")
        else:
            return int.to_bytes(self.cid, 1, "big")+b'FJ'+b'\x09'

    @staticmethod
    def get_empty_info_str():
        return chr(0)+IngredientContainer(2).get_empty_info_str()+chr(0)
    
    @staticmethod
    def get_empty_info_bytes():
        return b'\x00'+IngredientContainer(2).get_empty_info_bytes()+b'\x00'
    
    def get_short_patience(self):
        return int(self.patience//10)
    
    def get_tip(self):
        if self.food_judge:
            return 1000
        else:
            return int((100-self.initial_patience)/10*(self.patience)/10*(3**len(self.order_ingredient_container.get_list())))
    
    @staticmethod
    def decode_info_str(info_str):
        cid = ord(info_str[0])
        if info_str[1:3] == "FJ":
            result = Customer.create_food_judge(cid)
            return result
        else:
            order_ingredient_container = IngredientContainer(2).decode_info_str(info_str[1:3])
            patience = ord(info_str[3:])*10
            result = Customer(patience, [], cid=cid)
            result.order_ingredient_container = order_ingredient_container
            result.initial_patience = None
            return result
    
    @staticmethod
    def decode_info_bytes(info_bytes):
        cid = info_bytes[0]
        if info_bytes[1:3] == b'FJ':
            result = Customer.create_food_judge(cid)
            return result
        else:
            order_ingredient_container = IngredientContainer(2).decode_info_bytes(info_bytes[1:3])
            patience = info_bytes[3]*10
            result = Customer(patience, order_ingredient_list = order_ingredient_container.get_list(), cid=cid)
            result.initial_patience = None
            return result
    
    @staticmethod
    def random_customer():
        order_ingredient_list = [Ingredient(letter) for letter in random.sample(INGREDIENT_LETTERS, random.randint(1, 2))]
        return Customer(random.uniform(SETTINGS["CUSTOMER_PATIENCE_MIN"],
                                       SETTINGS["CUSTOMER_PATIENCE_MAX"]),
                        order_ingredient_list)
    
    def __str__(self):
        if not self.food_judge:
            return f"cid:{self.cid}p:{self.get_short_patience()}-{str(self.order_ingredient_container)}"
        else:
            return f"cid:{self.cid}p:{self.get_short_patience()}-FJ"

    def __repr__(self):
        return str(self)

class CustomerContainer:
    def __init__(self, initial_customer_list=[]):
        self.customers = [None for _ in range(3)]
        for customer in initial_customer_list:
            self.add_customer(customer)
    
    def add_customer(self, customer):
        for i, cus in enumerate(self.customers):
            if cus is None:
                self.customers[i] = customer
                return i
        return -1
    
    def get_list(self, remove_empty_slots=True):
        if remove_empty_slots:
            return [cus for cus in self.customers if cus is not None]
        else:
            return self.customers
    
    def get_customer_with_index(self, index, raise_index_error_on_empty=True):
        if raise_index_error_on_empty and self.customers[index] is None:
            raise IndexError()
        return self.customers[index]
    
    def get_customer_index(self, customer):
        return self.customers.index(customer)
    
    def get_info_str(self):
        return "".join(Customer.get_empty_info_str() if cus is None else cus.get_info_str() 
                       for cus in self.customers)
    
    def get_info_bytes(self):
        return b''.join(Customer.get_empty_info_bytes() if cus is None else cus.get_info_bytes()
                        for cus in self.customers)
    
    def remove_customer(self, customer):
        self.customers[self.customers.index(customer)] = None
    
    def get_customer_with_id(self, customer_id):
        for customer in self.get_list():
            if customer.cid == customer_id:
                return customer
        return None
    
    def __str__(self):
        return "|".join([str(cus) for cus in self.customers])

    def __repr__(self):
        return str(self)
    
    @staticmethod
    def decode_info_str(info_str):
        customers = []
        for i in range(3):
            if info_str[i*4:(i+1)*4] == Customer.get_empty_info_str():
                customers.append(None)
            else:
                customers.append(Customer.decode_info_str(info_str[i*4:(i+1)*4]))
        result = CustomerContainer([])
        result.customers = customers
        return result
    
    @staticmethod
    def decode_info_bytes(info_bytes):
        customers = []
        for i in range(3):
            if info_bytes[i*4:(i+1)*4] == Customer.get_empty_info_bytes():
                customers.append(None)
            else:
                customers.append(Customer.decode_info_bytes(info_bytes[i*4:(i+1)*4]))
        result = CustomerContainer([])
        result.customers = customers
        return result

class CookingState:
    def __init__(self, customer_container, ingredient_container, money):
        self.customer_container = customer_container
        self.ingredient_container = ingredient_container
        self.money = money
        self.lock = threading.Lock()
    
    def get_info_str(self):
        with self.lock:
            return self.customer_container.get_info_str()+\
                   self.ingredient_container.get_info_str()+\
                   "".join(chr(b) for b in int.to_bytes(self.money, 2, "big"))

    def get_info_bytes(self):
        with self.lock:
            return self.customer_container.get_info_bytes()+\
                   self.ingredient_container.get_info_bytes()+\
                   int.to_bytes(self.money, 2, "big")
    
    def __str__(self):
        return f"C[{str(self.customer_container)}]-I[{str(self.ingredient_container)}]-{self.money}"
    
    def __repr__(self):
        return str(self)
    
    @staticmethod
    def decode_info_str(info_str):
        customer_container = CustomerContainer.decode_info_str(info_str[:-6])
        ingredient_container = IngredientContainer(4).decode_info_str(info_str[-6:-2])
        money = int.from_bytes(bytes(info_str[-2:]),"big")
        return CookingState(customer_container, ingredient_container, money)
    
    @staticmethod
    def decode_info_bytes(info_bytes):
        customer_container = CustomerContainer.decode_info_bytes(info_bytes[:-6])
        ingredient_container = IngredientContainer(4).decode_info_bytes(info_bytes[-6:-2])
        money = int.from_bytes(info_bytes[-2:],"big")
        return CookingState(customer_container, ingredient_container, money)
    
class CookingSimulator:
    def __init__(self):
        self.customer_container = CustomerContainer(initial_customer_list=[])
        self.ingredient_container = IngredientContainer(4)
        self.cooking_state = CookingState(self.customer_container, self.ingredient_container, 0)
        self.lock = self.cooking_state.lock

        self.easy_mode = True
        self._game_state = "IDLE"
        self.current_tick = -1
        self.goal_money = 0
        
        self.cooked_customer_ids = []
        self.slow_cooked_customer_id = None

        self.future_actions = []
        self.future_cook_waits = []
        self.future_cook_wait_idx = 0

        self.future_action_periods = []
        self.future_hashes = []
        self.future_answers = []
        self.upcoming_food_judge = None

        self.sent_hashes = 0
        self.received_answers = 0
    
    @property
    def game_state(self):
        with self.lock:
            return self._game_state[:]
    
    @game_state.setter
    def game_state(self, value):
        with self.lock:
            self._game_state = value
    
    def get_visible_info_str(self):
        return self.cooking_state.get_info_str()

    def setup(self, easy_mode):
        self.easy_mode = easy_mode
        if self.easy_mode:
            total_ticks = (SETTINGS["TOTAL_TIME_MS"]-SETTINGS["ARRIVAL_TOLERANCE_MS"])//(1000//SETTINGS["FPS"])
            self.future_actions = self._generate_future_actions(total_ticks)
        else:
            all_total_ticks = 0
            for i in range(SETTINGS["NUM_FOOD_JUDGES"]+1):
                total_ticks =  random.randint(SETTINGS["FOOD_JUDGE_INTERVAL_TICKS_MIN"],
                                              SETTINGS["FOOD_JUDGE_INTERVAL_TICKS_MAX"])
                all_total_ticks += total_ticks
                future_actions = self._generate_future_actions(total_ticks)
                if i != SETTINGS["NUM_FOOD_JUDGES"]:
                    future_actions.append((total_ticks, "_add_food_judge",()))
                if i == 0:
                    self.future_actions = future_actions
                else:
                    self.future_action_periods.append(future_actions)
            
            self.upcoming_food_judge = Customer.create_food_judge()
            while len(self.future_hashes) < 3:
                key = "".join([random.choice(string.ascii_lowercase) for _ in range(SETTINGS["HASH_KEY_LENGTH"])])
                key = key.encode("ascii")
                assert(hashbytes_sanity_check(key))
                answer = hash_336(key.decode("ascii")).encode("ascii")
                assert(hashbytes_sanity_check(answer))
                self.future_hashes.append(key)
                self.future_answers.append(answer)

            assert(all_total_ticks < (SETTINGS["TOTAL_TIME_MS"]-SETTINGS["ARRIVAL_TOLERANCE_MS"])//(1000//SETTINGS["FPS"]))
        
        self.future_cook_waits = [random.randint(SETTINGS["COOK_WAIT_TICKS_MIN"],
                                                    SETTINGS["COOK_WAIT_TICKS_MAX"]) for _ in range(500)]
        
        def log_future_actions(future_actions, title):
            logging.debug(f"{title}\n"+"\n".join([str(a) for a in self.future_actions]))
        log_future_actions(self.future_actions, "Current Future Actions:")
        for i,future_actions in enumerate(self.future_action_periods):
            log_future_actions(future_actions, f"Future Action Period {i}:")
        
        logging.getLogger().disabled = True
        if self.easy_mode:
            self.goal_money = self.dry_run()*3//4
        else:
            self.goal_money = self.dry_run()
            food_judge_money =1000*SETTINGS["NUM_FOOD_JUDGES"]
            assert(self.goal_money > food_judge_money)
            self.goal_money = (self.goal_money-food_judge_money)*3//4+food_judge_money
        logging.getLogger().disabled = False
    
    def _generate_future_actions(self, total_ticks):
        ticks_passed = 0
        future_actions = []
        while True:
            interval = random.randint(SETTINGS["CUSTOMER_INTERVAL_TICKS_MIN"], SETTINGS["CUSTOMER_INTERVAL_TICKS_MAX"])
            if interval+ticks_passed > total_ticks:
                break
            ticks_passed += interval
            num_simultaneous_customers = random.randint(1,2)
            for i in range(num_simultaneous_customers):
                future_actions.append((ticks_passed+i*5, "_add_customer", (Customer.random_customer(),)))

        ticks_passed = 0
        while True:
            interval = random.randint(SETTINGS["INGREDIENT_INTERVAL_TICKS_MIN"], SETTINGS["INGREDIENT_INTERVAL_TICKS_MAX"])
            if interval+ticks_passed > total_ticks:
                break
            ticks_passed += interval
            future_actions.append((ticks_passed, "_add_ingredient", (Ingredient.random_ingredient(),)))
        
        future_actions.sort(key=lambda x: x[0])
        return future_actions
    
    def dry_run(self):
        sent_ids = []

        dry_simulator = CookingSimulator()
        dry_simulator.easy_mode = self.easy_mode
        dry_simulator.future_actions = copy.deepcopy(self.future_actions)
        dry_simulator.future_cook_waits = copy.deepcopy(self.future_cook_waits)

        dry_simulator.future_action_periods = copy.deepcopy(self.future_action_periods)
        dry_simulator.future_hashes = copy.deepcopy(self.future_hashes)
        dry_simulator.future_answers = copy.deepcopy(self.future_answers)
        dry_simulator.upcoming_food_judge = copy.deepcopy(self.upcoming_food_judge)

        def serve_food_judge(cooking_state):
            if cooking_state.ingredient_container.get_list() == []:
                return None
            for customer in cooking_state.customer_container.get_list():
                if customer.food_judge and customer.cid not in sent_ids:
                    for i,ing in enumerate(cooking_state.ingredient_container.ingredients):
                        if ing is not None and ing.cooking is None:
                            sent_ids.append(customer.cid)
                            hash_bytestr = dry_simulator.slow_cook(customer.cid, i)
                            dry_simulator.serve_slow_cooked_food(hash_336(hash_bytestr.decode("ascii")).encode("ascii"))
                            return (customer.cid, i)
            return None

        def send_food(cooking_state):
            ingredients = cooking_state.ingredient_container.ingredients
            for customer in sorted(cooking_state.customer_container.get_list(), key=lambda x: 10-x.patience):
                if customer.cid in sent_ids or customer.food_judge:
                    continue
                order_ingredients = customer.order_ingredient_container.get_list()
                if all(ing in ingredients for ing in order_ingredients):
                    sent_ids.append(customer.cid)
                    ingredient_indices = [ingredients.index(ing) for ing in order_ingredients]
                    dry_simulator.cook(customer.cid, ingredient_indices)
                    return (customer.cid, ingredient_indices)
            return None
        
        def toss_if_nothing_matches(cooking_state):
            ingredients = cooking_state.ingredient_container.get_list()
            if len(ingredients) < 4:
                return None
            customers = cooking_state.customer_container.get_list()
            for i,ing in enumerate(ingredients):
                if ing.cooking:
                    continue
                if all([ing not in customer.order_ingredient_container.get_list() for customer in customers]):
                    dry_simulator.toss(i)
                    return i
            
            #toss first non-cooking ingredient since it doesn't matter at this point
            for i,ing in enumerate(ingredients):
                if ing.cooking:
                    continue
                else:
                    dry_simulator.toss(i)
            return None

        dry_simulator.start()
        while not (dry_simulator.future_actions == []):
            dry_simulator.tick()
            response = serve_food_judge(dry_simulator.cooking_state)
            if response is None:
                response = send_food(dry_simulator.cooking_state)
            if response is None:
                toss_if_nothing_matches(dry_simulator.cooking_state)
        
        return dry_simulator.cooking_state.money

    def _add_customer(self, customer):
        self.customer_container.add_customer(customer)

    def _add_ingredient(self, ingredient):
        self.ingredient_container.add_ingredient(ingredient)

    def _serve_cooked_food(self, customer_id, ingredient_indices):
        customer = self.customer_container.get_customer_with_id(customer_id)
        logging.info(f"Serving customer with id {customer_id} with ing indices {ingredient_indices} at state {self.cooking_state}")
        if customer is None:
            logging.warning(f"customer with id {customer_id} had already left when the ingredients with indices"
                   f"{ingredient_indices} are served, all that food wasted :(")
        else:
            self.cooking_state.money += customer.get_tip()
            self.cooking_state.money = sanitize_bytewise(self.cooking_state.money, 2)
            assert(check_byte_sanity(self.cooking_state.money, 2))

            self.customer_container.remove_customer(customer)

        for i in ingredient_indices:
            self.ingredient_container.remove_ingredient(i)

    def serve_slow_cooked_food(self, answer_bytestr):
        with self.lock:
            if self.future_answers == []:
                raise HashReplyCommandException(answer_bytestr, "", "you should have solved all the hashes, why are you sending new answers?")
            if self.slow_cooked_customer_id is None:
                raise HashReplyCommandException(answer_bytestr, self.future_answers[0], "no valid slow cook command has been issued yet")
            if answer_bytestr != self.future_answers[0]:
                raise HashReplyCommandException(answer_bytestr, self.future_answers[0])
            
            logging.info(f"Serving slow cooked food with answer {answer_bytestr} for customer with id {self.slow_cooked_customer_id}")
            self.received_answers += 1
            for i, ing in enumerate(self.ingredient_container.ingredients):
                if ing is not None and ing.cooking == "S":
                    self.ingredient_container.remove_ingredient(i)
                    break
            customer = self.customer_container.get_customer_with_id(self.slow_cooked_customer_id)
           
            self.cooking_state.money += customer.get_tip()
            self.cooking_state.money = sanitize_bytewise(self.cooking_state.money, 2)
            self.customer_container.remove_customer(customer)
            assert(check_byte_sanity(self.cooking_state.money, 2))

            self.slow_cooked_customer_id = None
            self.future_answers = self.future_answers[1:]
            self.future_hashes = self.future_hashes[1:]
            self.upcoming_food_judge = Customer.create_food_judge()

            future_actions = self.future_action_periods[0]
            self.future_action_periods = self.future_action_periods[1:]
            self.future_actions.extend([(self.current_tick+tick, *_) for (tick, *_) in  future_actions])
            self.future_actions.sort()

    def start(self):
        with self.lock:
            self.current_tick = 0
    
    def tick(self):
        with self.lock:
            if self.current_tick == -1:
                return

            while len(self.future_actions) > 0 and self.future_actions[0][0] == self.current_tick:
                _, method_name, args = self.future_actions[0]
                if method_name == "_add_food_judge":
                    if len(self.customer_container.get_list()) == 3:
                        self.future_actions.append((self.current_tick+1, "_add_food_judge", ()))
                        self.future_actions.sort()
                    elif (self.ingredient_container.get_list() == []) or all([ing.cooking for ing in self.ingredient_container.get_list()]):
                        self.future_actions.extend([(self.current_tick+1, "_add_ingredient", (Ingredient.random_ingredient(),)),
                                                    (self.current_tick+2, "_add_food_judge", ())])
                        self.future_actions.sort()
                    else:
                        logging.debug("adding food judge"+str(self.upcoming_food_judge))
                        assert(self._add_food_judge() != -1)
                        logging.debug("results in state"+str(self.cooking_state))
                else:
                    logging.debug("taking action "+str(self.future_actions[0]))
                    getattr(self, method_name)(*args)
                    logging.debug("results in state "+str(self.cooking_state))

                self.future_actions = self.future_actions[1:]
            
            for customer in self.customer_container.get_list():
                if customer.food_judge:
                    continue
                customer.patience -= SETTINGS["CUSTOMER_PATIENCE_DECREMENT_PER_TICK"]
                if customer.get_short_patience() <= 0:
                    self.customer_container.remove_customer(customer)

            self.current_tick += 1
    
    def _add_food_judge(self):
        return self.customer_container.add_customer(self.upcoming_food_judge)

    def cook(self, customer_id, ingredient_indices):
        with self.lock:
            if customer_id in self.cooked_customer_ids:
                raise CookCommandException(customer_id, ingredient_indices, self.cooking_state, f"order of customer with id-{customer_id} is already being processed")
            customer = self.customer_container.get_customer_with_id(customer_id)
            if customer is None:
                raise CookCommandException(customer_id, ingredient_indices, self.cooking_state, "customer is not present")
            if customer.food_judge:
                raise CookCommandException(customer_id, ingredient_indices, self.cooking_state, "food judges will not eat common dishes")
            
            ingredients = []
            try:
                for i in ingredient_indices:
                    ingredient = self.ingredient_container.get_ingredient(i)
                    if ingredient.cooking:
                        raise CookCommandException(customer_id, ingredient_indices, self.cooking_state, f"ingredient with index {i} is already being cooked")
                    ingredients.append(ingredient)
            except IndexError as e:
                raise CookCommandException(customer_id, ingredient_indices, self.cooking_state, e.args[0])

            if set(customer.order_ingredient_container.get_list()) != set(ingredients):
                raise CookCommandException(customer_id, ingredient_indices, self.cooking_state, "customer did not order these ingredients")
            
            logging.info(f"Cooking for customer {customer_id} with ingredient indices {ingredient_indices} for state {self.cooking_state}")
            self.cooked_customer_ids.append(customer_id)
            for ing in ingredients:
                ing.cooking = "C"
            
            serve_action = (self.current_tick+self.future_cook_waits[self.future_cook_wait_idx],
                                        "_serve_cooked_food",
                                        (customer_id, ingredient_indices))
            self.future_actions.append(serve_action)
            self.future_actions.sort()
            self.future_cook_wait_idx = (self.future_cook_wait_idx+1) % 500


    def toss(self, ingredient_index):
        with self.lock:
            try:
                ingredient = self.ingredient_container.get_ingredient(ingredient_index)
                if ingredient.cooking:
                    raise TossCommandException(ingredient_index, self.cooking_state, "cannot throw away food in the oven")
                logging.info(f"tossing ingredient idx {ingredient_index} at state {self.cooking_state}")
                self.ingredient_container.remove_ingredient(ingredient_index)
            except IndexError as e:
                raise TossCommandException(ingredient_index, self.cooking_state, e.args[0])
    
    def slow_cook(self, customer_id, ingredient_index):
        with self.lock:
            if self.slow_cooked_customer_id is not None:
                raise SlowCookCommandException(customer_id, ingredient_index, self.cooking_state, f"Cannot slow-cook for multiple customers, customer with id-{self.slow_cooked_customer_id} is already being processed")
            if customer_id in self.cooked_customer_ids:
                raise SlowCookCommandException(customer_id, ingredient_index, self.cooking_state, f"order of customer with id-{customer_id} is already being processed")
            customer = self.customer_container.get_customer_with_id(customer_id)
            if customer is None:
                raise SlowCookCommandException(customer_id, ingredient_index, self.cooking_state, "customer is not present")
            if not customer.food_judge:
                raise SlowCookCommandException(customer_id, ingredient_index, self.cooking_state, "cannot serve a common customer a specialty dish")
            
            try:
                ingredient =self.ingredient_container.get_ingredient(ingredient_index)
                if ingredient.cooking:
                    raise SlowCookCommandException(customer_id, ingredient_index, self.cooking_state, f"ingredient with index {ingredient_index} is already being cooked")
            except IndexError as e:
                raise SlowCookCommandException(customer_id, ingredient_index, self.cooking_state, e.args[0])
            
            self.slow_cooked_customer_id = customer_id
            self.cooked_customer_ids.append(customer_id)
            ingredient.cooking = "S"
            hash_bytestr = self.future_hashes[0]
            logging.info(f"Slow-cooking for customer {customer_id} with ingredient index {ingredient_index} for state {self.cooking_state}, sending {hash_bytestr}, expecting {self.future_answers[0]}")
            self.sent_hashes += 1
            return hash_bytestr

