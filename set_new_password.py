#!/usr/bin/env python3
# coding: utf-8

from getpass import getpass
import usb.util  # pyusb==1.1.0

PASSWORD_MAX_LENGTH = 15
USB_VENDOR_REQ_SET_CHAR = 0x01
USB_PRODUCT_NAME = "STM32 USB Password Manager"


def main():
    dev = usb.core.find(idVendor=0x0483, idProduct=0x572b)

    if dev is None or dev.product != USB_PRODUCT_NAME:
        raise ValueError('Device not found')

    print("\n== Target device ==\n")
    print("  Manufacturer : %s" % dev.manufacturer)
    print("  Product      : %s" % dev.product)
    print("  Serial       : %s" % dev.serial_number)
    print("")

    password = getpass()

    if len(password) > PASSWORD_MAX_LENGTH:
        password = password[:PASSWORD_MAX_LENGTH]

    index = 0

    for ch in password:
        wValue = (ord(ch) << 8) | index
        dev.ctrl_transfer(usb.TYPE_VENDOR, USB_VENDOR_REQ_SET_CHAR, wValue=wValue, wIndex=0, data_or_wLength=0)
        index += 1

    dev.ctrl_transfer(usb.TYPE_VENDOR, USB_VENDOR_REQ_SET_CHAR, wValue=index, wIndex=0, data_or_wLength=0)


if __name__ == "__main__":
    main()
