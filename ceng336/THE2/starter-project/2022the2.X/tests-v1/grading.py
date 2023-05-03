import pickle
from dataclasses import dataclass

# Maximum number of instruction cycles during which all displays must be updated.
# 17e3 is 1e6/60 rounded up to nearest thousand
DISPLAY_CYCLES = 17_000

def bin8(value: int):
    """
    Convert 8-bit register value to binary representation.
    """
    return ("0"*8 + bin(value)[2:])[-8:]

def metronome_pattern(bar_length, period):
    remaining = period - 50e3
    return [(3, 50e3), (0, remaining)] + [(1, 50e3), (0, remaining)] * (bar_length - 1)

def timing_grade(time, target, margin):
    error = abs(target-time)
    if error <= margin:
        return 1.0
    else:
        return 0

@dataclass
class RegRecording:
    data: int
    time: int
    tainted: bool = False

    def timing_grade(self, target, margin):
        return timing_grade(self.time, target, margin)

def bitmask_history(history, bitmask: int):
    first = history[0]
    newhist = [RegRecording(first.data & bitmask, first.time, first.data & bitmask != first.data)]
    for n in history[1:]:
        if n.data & bitmask == newhist[-1].data:
            newhist[-1].time += n.time
            if n.data & bitmask != n.data:
                newhist[-1].tainted = True
        else:
            newhist.append(RegRecording(n.data & bitmask, n.time, n.data & bitmask != n.data))
    return newhist

def prefix_match(sequence, target):
    out = []
    for i, expected in enumerate(target):
        if i >= len(sequence):
            break
        if sequence[i].data != expected:
            break
        out.append(sequence[i])
    return out

def get_longest_match(sequence, target):
    prefixes = [prefix_match(sequence[i:], target) for i in range(len(sequence))]
    if not prefixes:
        return -1, []
    # Reversed because we want to prefer the latest longest match.
    # For example, if we don't use the latest match in 2X speed test, we might incorrectly
    # detect a timing error.
    maxi = max(reversed(range(len(prefixes))), key=lambda i: len(prefixes[i]))
    return maxi, prefixes[maxi]

class Report:
    def __init__(self, rubric: list):
        self.grades = {}
        self.rubric = rubric

    def set_grade(self, name: str, grade: float, print_grade: bool = True):
        self.grades[name] = grade
        matches = [r for r in self.rubric if r[0] == name]
        if len(matches) != 1:
            raise ValueError("Rubric error: " + str(matches))
        if print_grade:
            r = matches[0]
            scaled = round(grade * r[1], 2)
            print(f'Grade for "{r[0]}": {scaled:.2f} / {r[1]:.1f}')

    def print_grade(self, name: str):
        grade = self.grades[name]
        matches = [r for r in self.rubric if r[0] == name]
        if len(matches) != 1:
            raise ValueError("Rubric error: " + str(matches))
        r = matches[0]
        scaled = round(grade * r[1], 2)
        print(f'Grade for "{r[0]}": {scaled:.2f} / {r[1]:.1f}')

    def get_expect_func(self, name: str, items: int):
        correct = 0
        total = 0
        def func(test, info: str):
            nonlocal correct, total
            total += 1
            if test:
                print(info+": PASS")
                correct += 1
            else:
                print(info+": FAILED")
            grade = correct / items
            self.set_grade(name, grade, False)
            if total > items:
                print("GRADER:", name, "expect func total must be at least", total)
        return func

    def save_grades(self, filename):
        with open(filename, 'wb') as f:
            pickle.dump(self.grades, f)

    def load_grades(self, filename):
        with open(filename, 'rb') as f:
            self.grades = pickle.load(f)

    def report_items(self):
        def process_rubric_item(r):
            grade = self.grades[r[0]] if r[0] in self.grades else 0.0
            grade = round(grade * r[1], 2)
            return [r[0], grade, r[1]]
        grade_items = [process_rubric_item(g) for g in self.rubric]
        f = "{0:>5} / {1:>4}  {2}"
        total_grade = round(sum([g[1] for g in grade_items]), 2)
        max_grade = round(sum([g[1] for g in self.rubric]), 2)
        return [g + [f.format(g[1], g[2], g[0])] for g in grade_items], total_grade, max_grade

    def report_str(self):
        grade_items, total_grade, max_grade = self.report_items()
        grade_report = "\n".join([g[3] for g in grade_items])
        return f"GRADE\n{grade_report}\nTOTAL GRADE: {total_grade:.2f}/{max_grade:.2f}"

def read_display(m, max_cycles: int = DISPLAY_CYCLES):
    # Watch writes to this register
    m.watch("PORTA W")
    m.watch("LATA W")
    m.watch("PORTD W")
    m.watch("LATD W")
    segments = [ [0] * 8 for _ in range(4) ]
    display_cycles = [0] * 4
    def light_segments(s, d, time):
        for i in range(len(s)):
            if d & 2**i:
                s[i] += time
    lata = m.get("LATA")
    latd = m.get("LATD")
    cycles = 0
    while cycles < max_cycles:
        if not m.run_timeout():
            print("Timed out while reading 7-segment display!")
            break
        time = m.stopwatch()
        for i in range(4):
            if lata == 2**i:
                display_cycles[i] += time
                light_segments(segments[i], latd, time)
                break
        lata = m.get("LATA")
        latd = m.get("LATD")
        cycles += time
        if sum([i > 250 for i in display_cycles]) == 4:
            break
    m.clear_breakpoints()
    values = [
        sum([2**i if value > 200 else 0 for i, value in enumerate(display_segments)])
        for display_segments in segments
    ]
    return values

def record_register(m, register, max_history: int, max_cycles: int, flicker_duration: int = 1000):
    max_changes = max_history * 4
    # Watch writes to this register
    m.watch(register + " W")
    if register.startswith("LAT"):
        m.watch("PORT" + register[3] + " W")
    history = []
    value = m.get(register)
    current = RegRecording(value, 0)
    cycles = 0
    changes = 0
    while len(history) < max_history:
        if not m.run_timeout():
            print("Timed out while waiting for a", register, "change!")
            m.clear_breakpoints()
            current.time += m.stopwatch()
            history.append(current)
            return history
        value = m.get(register)
        time = m.stopwatch()
        current.time += time
        cycles += time
        if value != current.data:
            print(register, "changed from", str(current.data), "to", str(value) + " (" + bin8(value) + ") after", str(current.time/1e6) + "s", end = "")
            if current.time > flicker_duration:  # Don't record if too short
                history.append(current)
                print()
            else:
                print(" (FLICKER, ignored)")
            current = RegRecording(value, 0)
        if cycles > max_cycles:
            print(f"Aborting", register, "recording: Exceeded the maximum duration of {max_cycles} cycles")
            if current.time > flicker_duration or not history:
                history.append(current)
            break
        changes += 1
        # print("Changes:", changes)
        # ~175 changes
        if changes > max_changes:
            print(f"Aborting", register, "recording: Exceeded the maximum number of changes ({max_changes})")
            if current.time > flicker_duration or not history:
                history.append(current)
            break
    m.clear_breakpoints()
    return history

def click_button(m, pin):
    """
    Press the button, wait 10ms, release the button
    """
    print("Pressing & releasing "+ pin.upper() + "...")
    m.exec("write pin " + pin + " high")
    m.stepi(10_000)
    m.exec("write pin " + pin + " low")

def hold_button(m, pin):
    """
    Press the button
    """
    print("Holding "+ pin.upper() + "...")
    m.exec("write pin " + pin + " high")

def release_button(m, pin):
    """
    Release the button
    """
    print("Releasing "+ pin.upper() + "...")
    m.exec("write pin " + pin + " low")
