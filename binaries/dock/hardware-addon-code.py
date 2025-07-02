import time
import board
import digitalio

# Define the pins for the LEDs
led_pins = [
    board.IO16,
    board.IO17,
    board.IO18,
    board.IO38,
    board.IO39,
    board.IO40,
    board.IO41,
    board.IO42
]
GlobalCommInterval = 0.5
FlagString = "FLAG-Th1sSch3m@t1c1SM@dn3ss!"
EnableBitFlip = False
PinScrambler = "26438715"

# Initialize all LEDs
leds = []
for pin in led_pins:
    led = digitalio.DigitalInOut(pin)
    led.direction = digitalio.Direction.OUTPUT
    leds.append(led)

# Blink pattern function
def blink_all(duration=0.5):
    # Turn all LEDs on
    binaryvalue_to_led("11111111")

    # Turn all LEDs off
    binaryvalue_to_led("00000000")

def chase_pattern(duration=0.1):
    # Light each LED in sequence
    for led in range(8):
        real_led = (int(PinScrambler[led])-1)
        leds[real_led].value = True
        time.sleep(duration)
        leds[real_led].value = False

def reverse_chase_pattern(duration=0.1):
    # Light each LED in sequence
    for led in reversed(range(8)):
        real_led = (int(PinScrambler[led])-1)
        leds[real_led].value = True
        time.sleep(duration)
        leds[real_led].value = False


def init_pattern():
    blink_all(GlobalCommInterval)
    blink_all(GlobalCommInterval)
    binaryvalue_to_led("01010101")
    binaryvalue_to_led("10101010")
    blink_all(GlobalCommInterval)
    blink_all(GlobalCommInterval)

def end_pattern():
    blink_all(GlobalCommInterval)
    blink_all(GlobalCommInterval)
    binaryvalue_to_led("10101010")
    binaryvalue_to_led("01010101")
    blink_all(GlobalCommInterval)
    blink_all(GlobalCommInterval)


def bitflip(binvalue: string):
    Modifiedbinvalue = f'{int(binvalue,2) ^ (1 << 3):08b}'
    Currentbinvalue = binvalue
    print("BitFlip",Currentbinvalue,Modifiedbinvalue)
    
    return Modifiedbinvalue

def binaryvalue_to_led(binvalue: string):

    CurrentBinValue=binvalue

    if len(CurrentBinValue) != 8:
        print('Wrong len')    
    else:
        for char in range(8):
            real_led = (int(PinScrambler[char])-1)

            if char == 4:
                leds[real_led].value = int(not bool(int(CurrentBinValue[char])))
            else:    
                leds[real_led].value = int(CurrentBinValue[char])
    time.sleep(GlobalCommInterval)       
    
def showtext(displaytext: string):
    for char in displaytext:
        binaryvalue_to_led(f'{ord(char):08b}')
    
def test_pattern():
    binaryvalue_to_led("00000000")
    binaryvalue_to_led("10000000")
    binaryvalue_to_led("01000000")
    binaryvalue_to_led("00100000")
    binaryvalue_to_led("00010000")
    binaryvalue_to_led("00001000")
    binaryvalue_to_led("00000100")
    binaryvalue_to_led("00000010")
    binaryvalue_to_led("00000001")
    binaryvalue_to_led("00000000")    

# Main loop
while True:
    init_pattern()
    showtext(FlagString)
    end_pattern()
    time.sleep(2)