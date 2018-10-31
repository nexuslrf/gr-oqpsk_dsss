# -*- coding: utf-8 -*-



# convert a binary string with length 2^n to delta(f) string according to MSK
# for more information, see paper CMOS RFIC Architectures for IEEE 802.15.4 Networks
def convert(n,strin):
    bit_num=(1<<n)
    result_str=str()
    for i in range(bit_num-1):
        if(i%2!=0):  
            result_str+=str((eval(strin[i]))^(eval(strin[i+1])))
        else:
            if(((eval(strin[i]))^(eval(strin[i+1])))==0):
                result_str+='1'
            else:
                result_str+='0'
    return result_str

#a simple function to convert a binary string to the unsigned int
#for example convert_uint("1001")=9
def convert_uint(strin):
    count=0
    result_uint=0
    strin=strin[::-1]
    for i in strin:
        if (i=='1'):
            result_uint+=(1<<count)
        count+=1
    return result_uint





'''
doc=open("table2.txt",'w')


preConvert_list=['00010101',
 '00010110',
 '00011001',
 '00011010',
 '00100101',
 '00100110',
 '00101001',
 '00101010',
 '11010101',
 '11010110',
 '11011001',
 '11011010',
 '11100101',
 '11100110',
 '11101001',
 '11101010']


preConvert_list=['1110001111010010101001010001101100100110000011000101110011111001',
 '1111100010100111100100111010010000001110011101100101100101001001',
 '0000110110111001101001101000011011110100010110110110001101011000',
 '0001100100001100101011001111011011010011111000010101101000100111',
 '0010011000001100010111001111100111100011110100101010010100011011',
 '0000111001110110010110010100100111111000101001111001001110100100',
 '1111010001011011011000110101100000001101101110011010011010000110',
 '1101001111100001010110100010011100011001000011001010110011110110',
 '1101000011100001100101100010100000010101001111110110111111001010',
 '1100101110010100101000001001011100111101010001010110101001111010',
 '0011111010001010100101011011010111000111011010000101000001101011',
 '0010101000111111100111111100010111100000110100100110100100010100',
 '0001010100111111011011111100101011010000111000011001011000101000',
 '0011110101000101011010100111101011001011100101001010000010010111',
 '1100011101101000010100000110101100111110100010101001010110110101',
 '1110000011010010011010010001010000101010001111111001111111000101']

result_list=[]
for bin_stream in preConvert_list:
    result_list.append(convert(6,bin_stream))

for result_stream in result_list:
    print (result_stream,file=doc)
doc.close()


#the following data is from ieee802.15.4-2011 table 74
result_list=[]
result_list.append(convert(4,'0011111000100101'))
result_list.append(convert(4,'0100111110001001'))
result_list.append(convert(4,'0101001111100010'))
result_list.append(convert(4,'1001010011111000'))
result_list.append(convert(4,'0010010100111110'))
result_list.append(convert(4,'1000100101001111'))
result_list.append(convert(4,'1110001001010011'))
result_list.append(convert(4,'1111100010010100'))
result_list.append(convert(4,'0110101101110000'))
result_list.append(convert(4,'0001101011011100'))
result_list.append(convert(4,'0000011010110111'))
result_list.append(convert(4,'1100000110101101'))
result_list.append(convert(4,'0111000001101011'))
result_list.append(convert(4,'1101110000011010'))
result_list.append(convert(4,'1011011100000110'))
result_list.append(convert(4,'1010110111000001'))

#bit_num=2^5
print ('Data Symbol \t after MSK ')
for i in range(len(result_list)):
    print (i,'\t',result_list[i])
'''

