import time
import board
import digitalio

# Define the pins for the LEDs
led_pins = [
    #board.GPIO35,
    #board.GPIO36,
    #board.GPIO37,
    ## Had to change the below from GPIO to IO - padraignix
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
#FlagString = "abcdefghijklmn"
EnableBitFlip = False
#PinScrambler = "12345678" #unscrambled
PinScrambler = "26438715"


#to display a message with the LED, its important to use either functiuons:
# - binaryvalue_to_led (for a specific Binary pattern)
# - showtext (for a phrase, or word)

# Initialize all LEDs
leds = []
for pin in led_pins:
    led = digitalio.DigitalInOut(pin)
    led.direction = digitalio.Direction.OUTPUT
    leds.append(led)

#initialise inverted pin
#leds[4]=true

# Blink pattern function
def blink_all(duration=0.5):
    # Turn all LEDs on
    #for led in leds:
    #    led.value = True
    binaryvalue_to_led("11111111")
#    time.sleep(duration)
    
    # Turn all LEDs off
    #for led in leds:
    #    led.value = False
    binaryvalue_to_led("00000000")
#    time.sleep(duration)

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
#    chase_pattern(0.1)
#    reverse_chase_pattern(0.1)
    binaryvalue_to_led("01010101")
    binaryvalue_to_led("10101010")
    blink_all(GlobalCommInterval)
    blink_all(GlobalCommInterval)

def end_pattern():
    blink_all(GlobalCommInterval)
    blink_all(GlobalCommInterval)
#    chase_pattern(0.1)
#    reverse_chase_pattern(0.1)
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

#if EnableBitFlip:
#        CurrentBinValue=bitflip(binvalue)
#    else:
    CurrentBinValue=binvalue

    if len(CurrentBinValue) != 8:
        print('Wrong len')    
    else:
        for char in range(8):
            real_led = (int(PinScrambler[char])-1)
#            print("real_led",real_led)

            if char == 4:
                #print("LED 4 fix:",bool(int(CurrentBinValue[char])),int(not bool(int(CurrentBinValue[char]))))
                leds[real_led].value = int(not bool(int(CurrentBinValue[char])))
            else:    
                leds[real_led].value = int(CurrentBinValue[char])
            #leds[real_led].value = int(CurrentBinValue[char])
    time.sleep(GlobalCommInterval)       
    
def showtext(displaytext: string):
    for char in displaytext:
        binaryvalue_to_led(f'{ord(char):08b}')
    #time.sleep(2)
    
def test_pattern():
    #binaryvalue_to_led("01010101")
    #binaryvalue_to_led("10101010")
    #binaryvalue_to_led("00011000")
    #binaryvalue_to_led("00100100")
    #binaryvalue_to_led("01000010")
    #binaryvalue_to_led("10000001")
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
    # Blink all LEDs together 3 times
    #for _ in range(3):
    #    blink_all(0.5)
    
    # Do a chase pattern twice
    #for _ in range(2):
    #    chase_pattern(0.1)
    
    
    init_pattern()
    #test_pattern()
    # Small pause between patterns
    showtext(FlagString)
    end_pattern()
    time.sleep(2)