# Description for convert scripts：

## 1. convert2complex.py:

该程序是将每一个data symbol对应的chip values（对于16位来说就是，$c_0c_1c_2...c_{14}c_{15}$)转化为对应的复数表。其中需要注意的一点是由于data symbol之前做过翻转操作，此处需要先把其翻转回来，即对于data symbol = 2$(0010)$来说,它位于complex table的第8$(0100)$行

## 2. convert2MSK.py:

This py file realizes the basic convert according to the paper **CMOS RFIC Architectures For IEEE 802.15.4 Networks**. The input is I-Q binary stream in Figure 1. Symbol Set Diagram. The output is k-binary-stream, which is used to decide the carrier frequency delta(f) in each bit interval.

The example convert table can be seen in paper
**GNU Radio 802.15.4 En- and Decoding**    table 1 and 2 

Usage example:
convert("1001")