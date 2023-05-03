from mdb import *
from grading import *

# Timeout for waiting for program to arrive to main_loop label
MAIN_LOOP_TIMEOUT = 10000


TIMING_ERROR_MARGIN = 2e3

prelude =  """
device PIC18F4620
set warningmessagebreakoptions.W0223_ADC_UNDERFLOW ignore
set warningmessagebreakoptions.W0222_ADC_OVERFLOW ignore
set oscillator.frequency 1
set oscillator.frequencyunit Mega
hwtool sim
"""

breakpoints = [
    "main_loop",
    "timer0_interrupt",
]

rubric = [
    ["RB Port Change Interrupt", 5.0],
    ["Timer interrupts enabled", 5.0],

    ["Speed level 7-segment display", 17.5],
    ["Speed level 1", 2.5],
    ["Speed level 2", 2.5],
    ["Speed level 3", 2.5],
    ["Speed level 4", 2.5],
    ["Speed level 5", 2.5],
    ["Speed level 6", 2.5],
    ["Speed level 7", 2.5],
    ["Speed level 8", 2.5],
    ["Speed level 9", 2.5],

    ["Bar length", 40.0],
]

seven_seg_char = {
    '0': 0b00111111,
    '1': 0b00000110,
    '2': 0b01011011,
    '3': 0b01001111,
    '4': 0b01100110,
    '5': 0b01101101,
    '6': 0b01111101,
    '7': 0b00000111,
    '8': 0b01111111,
    '9': 0b01101111,
    'P': 0b01110011,
    ' ': 0,
    '-': 0b01000000,
}

def expect_display(expect, displays, expected):
    for i, (display, char) in enumerate(zip(displays, expected)):
        expect(display == seven_seg_char[char], f"DIS{i} must display '{char}'")

def report_grade(name: str, correct, max_correct):
    global report
    print("Correct/Max:", f"{correct} / {max_correct}")
    report.set_grade(name, correct / max_correct)

def metronome_check(name: str, history, expected):
    grades = [a.timing_grade(e[1], TIMING_ERROR_MARGIN) * (1.0 if a.data == e[0] else 0.0) for a, e in zip(history, expected)]
    expected_value = [hex(e[0])[-1] for e in expected]
    expected_time = [e[1] for e in expected]
    print("Expected:")
    print("Value:    ", " ".join(["{:>4}".format(e) for e in expected_value]))
    print("Time (ms):", " ".join(["{:>4}".format(str(int(e / 1000))) for e in expected_time]))
    print("Output:")
    print("Value:    ", " ".join(["{:>4}".format(hex(e.data)[-1]) for e in history]))
    print("Time (ms):", " ".join(["{:>4}".format(str(e.time // 1000)) for e in history]))
    print("Points:   ", " ".join(["{:>4}".format(grade) for grade in grades]))
    report_grade(name, sum(grades), len(expected))

#
# TESTS
#

def rbie_test(m, bp2addr, addr2bp):
    """
    Checks whether the PORTB inputs are handled via interrupts.
    """
    global report
    expect = report.get_expect_func("RB Port Change Interrupt", 8)

    m.breakpoint(bp2addr["main_loop"])
    m.run(MAIN_LOOP_TIMEOUT)
    m.clear_breakpoints()

    intcon = m.get("INTCON")
    print("INTCON:", bin(intcon))
    expect(intcon & 8, "RBIE must be set when main_loop is reached")
    expect(intcon & 1 == 0, "RBIF must be cleared when main_loop is reached")

    m.breakpoint("8")

    def test(button):
        print()
        button_name = button.upper()
        click_button(m, button)
        bp = m.run_timeout(MAIN_LOOP_TIMEOUT)
        expect(bp and int(bp, 16) == 8, "Must hit 0x08 breakpoint after clicking " + button_name)
        m.stepi(10_000)

    test("rb7")
    test("rb6")
    test("rb5")
    test("rb4")

    m.stepi(10_000)
    intcon = m.get("INTCON")
    print("INTCON:", bin(intcon))
    expect(intcon & 8, "RBIE must be set when the metronome starts")
    expect(intcon & 1 == 0, "RBIF must be cleared when the metronome starts")

    expect = report.get_expect_func("Timer interrupts enabled", 2)
    expect(m.get("INTCON") & 32, "TMR0IE must be set when the metronome starts")
    expect(m.get("PIE1") & 1, "TMR1IE must be set when the metronome starts")


def speed_level_test(m, bp2addr, addr2bp):
    global report
    expect = report.get_expect_func("Speed level 7-segment display", 88)

    def test_speed_level(level):
        time = 100_000 * (10 - level + 1)
        m.stepi(10_000)
        expect_display(expect, read_display(m), "P  "+str(level))

        click_button(m, "rb4")
        history = record_register(m, "LATC", 16, 32 * time)
        expected = metronome_pattern(4, time) * 2
        metronome_check("Speed level "+str(level), history, expected)

        click_button(m, "rb4")
        m.stepi(10_000)
        expect_display(expect, read_display(m), "P  "+str(level))

    m.breakpoint(bp2addr["main_loop"])
    m.run(MAIN_LOOP_TIMEOUT)
    m.clear_breakpoints()

    test_speed_level(6)
    click_button(m, "rb6")
    test_speed_level(5)
    click_button(m, "rb6")
    test_speed_level(4)
    click_button(m, "rb6")
    test_speed_level(3)
    click_button(m, "rb6")
    test_speed_level(2)
    click_button(m, "rb6")
    test_speed_level(1)
    click_button(m, "rb6")
    m.stepi(10_000)
    expect_display(expect, read_display(m), "P  1")

    click_button(m, "rb7")
    test_speed_level(6)
    click_button(m, "rb5")
    test_speed_level(7)
    click_button(m, "rb5")
    test_speed_level(8)
    click_button(m, "rb5")
    test_speed_level(9)
    click_button(m, "rb5")
    m.stepi(10_000)
    expect_display(expect, read_display(m), "P  9")


def bar_length_test(m, bp2addr, addr2bp):
    global report
    expect = report.get_expect_func("Bar length", 316)

    m.breakpoint(bp2addr["main_loop"])
    m.run(MAIN_LOOP_TIMEOUT)
    m.clear_breakpoints()

    def expect_sequence(sequence):
        for latc, display in sequence:
            # read_display removes the breakpoint
            m.breakpoint(bp2addr["timer0_interrupt"])
            m.run()
            m.stepi(10_000)
            expect(m.get("LATC") == latc, f"LATC must be {latc}")
            expect_display(expect, read_display(m), display)
            m.stepi(50_000)
            expect(m.get("LATC") == 0, "LATC must be 0 after 50 ms")

    m.stepi(10_000)
    click_button(m, "rb4")
    m.stepi(10_000)
    expect(m.get("LATC") == 3, "LATC must be 3 at the start of a bar")
    expect_display(expect, read_display(m), " 1-4")
    m.stepi(50_000)
    expect(m.get("LATC") == 0, "LATC must be 0 after 50 ms")
    expect_sequence([
        (1, " 2-4"),
        (1, " 3-4"),
        (1, " 4-4"),
        (3, " 1-4"),
    ])

    click_button(m, "rb6")
    m.stepi(50_000)
    expect_display(expect, read_display(m), " 1-3")
    expect_sequence([
        (1, " 2-3"),
        (1, " 3-3"),
        (3, " 1-3"),
    ])

    click_button(m, "rb6")
    m.stepi(50_000)
    expect_display(expect, read_display(m), " 1-2")
    expect_sequence([
        (1, " 2-2"),
        (3, " 1-2"),
    ])

    click_button(m, "rb6")
    m.stepi(50_000)
    expect_display(expect, read_display(m), " 1-2")
    expect_sequence([
        (1, " 2-2"),
        (3, " 1-2"),
    ])

    click_button(m, "rb7")
    m.stepi(50_000)
    expect_display(expect, read_display(m), " 1-4")
    expect_sequence([
        (1, " 2-4"),
        (1, " 3-4"),
        (1, " 4-4"),
        (3, " 1-4"),
    ])

    click_button(m, "rb5")
    m.stepi(50_000)
    expect_display(expect, read_display(m), " 1-5")
    expect_sequence([
        (1, " 2-5"),
        (1, " 3-5"),
        (1, " 4-5"),
        (1, " 5-5"),
        (3, " 1-5"),
    ])

    click_button(m, "rb5")
    m.stepi(50_000)
    expect_display(expect, read_display(m), " 1-6")
    expect_sequence([
        (1, " 2-6"),
        (1, " 3-6"),
        (1, " 4-6"),
        (1, " 5-6"),
        (1, " 6-6"),
        (3, " 1-6"),
    ])

    click_button(m, "rb5")
    m.stepi(50_000)
    expect_display(expect, read_display(m), " 1-7")
    expect_sequence([
        (1, " 2-7"),
        (1, " 3-7"),
        (1, " 4-7"),
        (1, " 5-7"),
        (1, " 6-7"),
        (1, " 7-7"),
        (3, " 1-7"),
    ])

    click_button(m, "rb5")
    m.stepi(50_000)
    expect_display(expect, read_display(m), " 1-8")
    expect_sequence([
        (1, " 2-8"),
        (1, " 3-8"),
        (1, " 4-8"),
        (1, " 5-8"),
        (1, " 6-8"),
        (1, " 7-8"),
        (1, " 8-8"),
        (3, " 1-8"),
    ])

    click_button(m, "rb5")
    m.stepi(50_000)
    expect_display(expect, read_display(m), " 1-8")
    expect_sequence([ (1, " 2-8") ])

    click_button(m, "rb7")
    m.stepi(50_000)
    expect_display(expect, read_display(m), " 2-4")
    expect_sequence([
        (1, " 3-4"),
        (1, " 4-4"),
        (3, " 1-4"),
    ])


if __name__ == "__main__":
    report = Report(rubric)

    tester = MdbTester(prelude, breakpoints)
    tester.run([
        rbie_test,
        speed_level_test,
        bar_length_test,
    ])

    print()
    print(report.report_str())
    report.save_grades("grades.pkl")
