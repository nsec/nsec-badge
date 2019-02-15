#!/usr/bin/env python3

# Client for the "flash mode" on the badge.

# This class can be used to send commands to the badge while it is listening
# in "flash mode" in order to write the external flash.
#
# The general form of commands understood by the flash mode is:
#
#   pc -> badge: <command> <some args>
#
# On success, the badge replies:
#
#  pc <- badge: <command> ok <possible some results>
#
# On error, it provides an error message:
#
#  pc <- badge: <command> error <an error message>
#
# The protocol is all line-based, and each message, each line ends with a \n.

import serial
import binascii


class FlashClient:
    def __init__(self, ser):
        self._ser = ser
        # Send the magic byte that clears the command buffer.
        self._ser.write(b'\x00')

        # Drain any data that might be coming our way.
        prev_timeout = self._ser.timeout
        self._ser.timeout = 0
        while self._ser.read():
            pass
        self._ser.timeout = prev_timeout

    def _run_command(self, cmd, args=''):
        if type(cmd) == str:
            cmd = cmd.encode()

        if type(args) == str:
            args = args.encode()

        if cmd.find(b' ') >= 0:
            raise ValueError(
                "The command should not contain spaces, did you mean to use args?")

        full_cmd = cmd

        if len(args) > 0:
            full_cmd = full_cmd + b' ' + args

        full_cmd = full_cmd + b'\n'

        print('pc -> badge: ', full_cmd)
        self._ser.write(full_cmd)

        ok_msg = cmd + b' ok'
        error_msg = cmd + b' error'

        while True:
            line = self._ser.readline()
            print('pc <- badge: ', line)

            if line.startswith(error_msg):
                raise RuntimeError(line[len(error_msg) + 1:])

            if line.startswith(ok_msg):
                return line[len(ok_msg) + 1:]

    def erase(self):
        """Erase the whole chip."""
        self._run_command('erase')

    def write(self, address, data):
        """Write 128 bytes of data.

        address must be an int, data must be a 128-bytes long bytes object.

        Raise RuntimeError if the checksum reported by the target does not match the
        checksum computed locally."""

        if type(address) != int:
            raise TypeError('address should be an int.')

        if type(data) != bytes:
            raise TypeError('data should be a bytes object.')

        if len(data) != 128:
            raise ValueError('data should be 128-bytes long.')

        address_hex = hex(address)
        data_hex = data.hex()
        args = '{} {}'.format(address_hex, data_hex)

        target_checksum_hex = self._run_command('write', args)
        target_checksum = int(target_checksum_hex, 16)

        our_checksum = binascii.crc32(data)
        if our_checksum != target_checksum:
            raise RuntimeError('checksums do not match')

    def read(self, address):
        """Reads 128 bytes of data.

        address must be an int.

        Returns a bytes object"""

        if type(address) != int:
            raise TypeError('address should be an int.')

        address_hex = hex(address)
        data_hex = self._run_command('read', address_hex)

        return bytes.fromhex(data_hex.decode())

    def checksum(self, address, length):
        """Computes the CRC32 of a region of flash.

        address and length must be ints.  length must be a multiple of 128.

        Returns the CRC32 as an int."""

        if type(address) != int:
            raise TypeError('address should be an int.')

        if type(length) != int:
            raise TypeError('length should be an int.')

        if length % 128 != 0:
            raise ValueError('length should be a multiple of 128')

        address_hex = hex(address)
        length_hex = hex(length)
        args = '{} {}'.format(address_hex, length_hex)

        checksum_hex = self._run_command('checksum', args)

        return int(checksum_hex, 16)


def main():
    # This is an example of using this client to program the flash on the badge.
    with serial.Serial('/dev/ttyACM1', 38400) as ser:
        client = FlashClient(ser)

        client.erase()

        # 128 bytes of CAFEBABE
        data = b'\xca\xfe\xba\xbe' * 32
        client.write(0x200, data)

        data = client.read(0x200)
        assert data == b'\xca\xfe\xba\xbe' * 32
        data = client.read(0x1c0)
        assert data == b'\xff\xff\xff\xff' * 16 + b'\xca\xfe\xba\xbe' * 16
        data = client.read(0x240)
        assert data == b'\xca\xfe\xba\xbe' * 16 + b'\xff\xff\xff\xff' * 16

        target_checksum = client.checksum(0x240, 128)
        our_checksum = binascii.crc32(
            b'\xca\xfe\xba\xbe' * 16 + b'\xff\xff\xff\xff' * 16)
        assert target_checksum == our_checksum


if __name__ == '__main__':
    main()
