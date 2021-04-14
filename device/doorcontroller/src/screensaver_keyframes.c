#include <pgmspace.h>
#include <stdint.h>
#include <string.h>

const size_t key_frame_count = 360;
uint16_t key_frames[] = {
    288, 152, // k=1, Θ=0.0°
    288, 155, // k=2, Θ=1.0°
    288, 157, // k=3, Θ=2.0°
    288, 160, // k=4, Θ=3.0°
    288, 163, // k=5, Θ=4.0°
    287, 165, // k=6, Θ=5.0°
    287, 168, // k=7, Θ=6.0°
    287, 171, // k=8, Θ=7.0°
    287, 173, // k=9, Θ=8.0°
    286, 176, // k=10, Θ=9.0°
    286, 178, // k=11, Θ=10.0°
    285, 181, // k=12, Θ=11.0°
    285, 184, // k=13, Θ=12.0°
    284, 186, // k=14, Θ=13.0°
    284, 189, // k=15, Θ=14.0°
    283, 191, // k=16, Θ=15.0°
    282, 194, // k=17, Θ=16.0°
    282, 196, // k=18, Θ=17.0°
    281, 199, // k=19, Θ=18.0°
    280, 201, // k=20, Θ=19.0°
    279, 204, // k=21, Θ=20.0°
    278, 206, // k=22, Θ=21.0°
    278, 209, // k=23, Θ=22.0°
    277, 211, // k=24, Θ=23.0°
    276, 214, // k=25, Θ=24.0°
    275, 216, // k=26, Θ=25.0°
    273, 219, // k=27, Θ=26.0°
    272, 221, // k=28, Θ=27.0°
    271, 223, // k=29, Θ=28.0°
    270, 226, // k=30, Θ=29.0°
    269, 228, // k=31, Θ=30.0°
    267, 230, // k=32, Θ=31.0°
    266, 233, // k=33, Θ=32.0°
    265, 235, // k=34, Θ=33.0°
    263, 237, // k=35, Θ=34.0°
    262, 239, // k=36, Θ=35.0°
    260, 241, // k=37, Θ=36.0°
    259, 243, // k=38, Θ=37.0°
    257, 246, // k=39, Θ=38.0°
    256, 248, // k=40, Θ=39.0°
    254, 250, // k=41, Θ=40.0°
    253, 252, // k=42, Θ=41.0°
    251, 254, // k=43, Θ=42.0°
    249, 256, // k=44, Θ=43.0°
    248, 258, // k=45, Θ=44.0°
    246, 259, // k=46, Θ=45.0°
    244, 261, // k=47, Θ=46.0°
    242, 263, // k=48, Θ=47.0°
    240, 265, // k=49, Θ=48.0°
    238, 267, // k=50, Θ=49.0°
    237, 268, // k=51, Θ=50.0°
    235, 270, // k=52, Θ=51.0°
    233, 272, // k=53, Θ=52.0°
    231, 273, // k=54, Θ=53.0°
    229, 275, // k=55, Θ=54.0°
    227, 277, // k=56, Θ=55.0°
    225, 278, // k=57, Θ=56.0°
    222, 279, // k=58, Θ=57.0°
    220, 281, // k=59, Θ=58.0°
    218, 282, // k=60, Θ=59.0°
    216, 284, // k=61, Θ=60.0°
    214, 285, // k=62, Θ=61.0°
    212, 286, // k=63, Θ=62.0°
    209, 287, // k=64, Θ=63.0°
    207, 289, // k=65, Θ=64.0°
    205, 290, // k=66, Θ=65.0°
    203, 291, // k=67, Θ=66.0°
    200, 292, // k=68, Θ=67.0°
    198, 293, // k=69, Θ=68.0°
    196, 294, // k=70, Θ=69.0°
    193, 295, // k=71, Θ=70.0°
    191, 296, // k=72, Θ=71.0°
    188, 297, // k=73, Θ=72.0°
    186, 297, // k=74, Θ=73.0°
    184, 298, // k=75, Θ=74.0°
    181, 299, // k=76, Θ=75.0°
    179, 299, // k=77, Θ=76.0°
    176, 300, // k=78, Θ=77.0°
    174, 301, // k=79, Θ=78.0°
    171, 301, // k=80, Θ=79.0°
    169, 302, // k=81, Θ=80.0°
    167, 302, // k=82, Θ=81.0°
    164, 303, // k=83, Θ=82.0°
    162, 303, // k=84, Θ=83.0°
    159, 303, // k=85, Θ=84.0°
    157, 303, // k=86, Θ=85.0°
    154, 304, // k=87, Θ=86.0°
    152, 304, // k=88, Θ=87.0°
    149, 304, // k=89, Θ=88.0°
    147, 304, // k=90, Θ=89.0°
    144, 304, // k=91, Θ=90.0°
    141, 304, // k=92, Θ=91.0°
    139, 304, // k=93, Θ=92.0°
    136, 304, // k=94, Θ=93.0°
    134, 304, // k=95, Θ=94.0°
    131, 303, // k=96, Θ=95.0°
    129, 303, // k=97, Θ=96.0°
    126, 303, // k=98, Θ=97.0°
    124, 303, // k=99, Θ=98.0°
    121, 302, // k=100, Θ=99.0°
    119, 302, // k=101, Θ=100.0°
    117, 301, // k=102, Θ=101.0°
    114, 301, // k=103, Θ=102.0°
    112, 300, // k=104, Θ=103.0°
    109, 299, // k=105, Θ=104.0°
    107, 299, // k=106, Θ=105.0°
    104, 298, // k=107, Θ=106.0°
    102, 297, // k=108, Θ=107.0°
    100, 297, // k=109, Θ=108.0°
    97, 296, // k=110, Θ=109.0°
    95, 295, // k=111, Θ=110.0°
    92, 294, // k=112, Θ=111.0°
    90, 293, // k=113, Θ=112.0°
    88, 292, // k=114, Θ=113.0°
    85, 291, // k=115, Θ=114.0°
    83, 290, // k=116, Θ=115.0°
    81, 289, // k=117, Θ=116.0°
    79, 287, // k=118, Θ=117.0°
    76, 286, // k=119, Θ=118.0°
    74, 285, // k=120, Θ=119.0°
    72, 284, // k=121, Θ=120.0°
    70, 282, // k=122, Θ=121.0°
    68, 281, // k=123, Θ=122.0°
    66, 279, // k=124, Θ=123.0°
    63, 278, // k=125, Θ=124.0°
    61, 277, // k=126, Θ=125.0°
    59, 275, // k=127, Θ=126.0°
    57, 273, // k=128, Θ=127.0°
    55, 272, // k=129, Θ=128.0°
    53, 270, // k=130, Θ=129.0°
    51, 268, // k=131, Θ=130.0°
    50, 267, // k=132, Θ=131.0°
    48, 265, // k=133, Θ=132.0°
    46, 263, // k=134, Θ=133.0°
    44, 261, // k=135, Θ=134.0°
    42, 259, // k=136, Θ=135.0°
    40, 258, // k=137, Θ=136.0°
    39, 256, // k=138, Θ=137.0°
    37, 254, // k=139, Θ=138.0°
    35, 252, // k=140, Θ=139.0°
    34, 250, // k=141, Θ=140.0°
    32, 248, // k=142, Θ=141.0°
    31, 246, // k=143, Θ=142.0°
    29, 243, // k=144, Θ=143.0°
    28, 241, // k=145, Θ=144.0°
    26, 239, // k=146, Θ=145.0°
    25, 237, // k=147, Θ=146.0°
    23, 235, // k=148, Θ=147.0°
    22, 233, // k=149, Θ=148.0°
    21, 230, // k=150, Θ=149.0°
    19, 228, // k=151, Θ=150.0°
    18, 226, // k=152, Θ=151.0°
    17, 223, // k=153, Θ=152.0°
    16, 221, // k=154, Θ=153.0°
    15, 219, // k=155, Θ=154.0°
    13, 216, // k=156, Θ=155.0°
    12, 214, // k=157, Θ=156.0°
    11, 211, // k=158, Θ=157.0°
    10, 209, // k=159, Θ=158.0°
    10, 206, // k=160, Θ=159.0°
    9, 204, // k=161, Θ=160.0°
    8, 201, // k=162, Θ=161.0°
    7, 199, // k=163, Θ=162.0°
    6, 196, // k=164, Θ=163.0°
    6, 194, // k=165, Θ=164.0°
    5, 191, // k=166, Θ=165.0°
    4, 189, // k=167, Θ=166.0°
    4, 186, // k=168, Θ=167.0°
    3, 184, // k=169, Θ=168.0°
    3, 181, // k=170, Θ=169.0°
    2, 178, // k=171, Θ=170.0°
    2, 176, // k=172, Θ=171.0°
    1, 173, // k=173, Θ=172.0°
    1, 171, // k=174, Θ=173.0°
    1, 168, // k=175, Θ=174.0°
    1, 165, // k=176, Θ=175.0°
    0, 163, // k=177, Θ=176.0°
    0, 160, // k=178, Θ=177.0°
    0, 157, // k=179, Θ=178.0°
    0, 155, // k=180, Θ=179.0°
    0, 152, // k=181, Θ=180.0°
    0, 149, // k=182, Θ=181.0°
    0, 147, // k=183, Θ=182.0°
    0, 144, // k=184, Θ=183.0°
    0, 141, // k=185, Θ=184.0°
    1, 139, // k=186, Θ=185.0°
    1, 136, // k=187, Θ=186.0°
    1, 133, // k=188, Θ=187.0°
    1, 131, // k=189, Θ=188.0°
    2, 128, // k=190, Θ=189.0°
    2, 126, // k=191, Θ=190.0°
    3, 123, // k=192, Θ=191.0°
    3, 120, // k=193, Θ=192.0°
    4, 118, // k=194, Θ=193.0°
    4, 115, // k=195, Θ=194.0°
    5, 113, // k=196, Θ=195.0°
    6, 110, // k=197, Θ=196.0°
    6, 108, // k=198, Θ=197.0°
    7, 105, // k=199, Θ=198.0°
    8, 103, // k=200, Θ=199.0°
    9, 100, // k=201, Θ=200.0°
    10, 98, // k=202, Θ=201.0°
    10, 95, // k=203, Θ=202.0°
    11, 93, // k=204, Θ=203.0°
    12, 90, // k=205, Θ=204.0°
    13, 88, // k=206, Θ=205.0°
    15, 85, // k=207, Θ=206.0°
    16, 83, // k=208, Θ=207.0°
    17, 81, // k=209, Θ=208.0°
    18, 78, // k=210, Θ=209.0°
    19, 76, // k=211, Θ=210.0°
    21, 74, // k=212, Θ=211.0°
    22, 71, // k=213, Θ=212.0°
    23, 69, // k=214, Θ=213.0°
    25, 67, // k=215, Θ=214.0°
    26, 65, // k=216, Θ=215.0°
    28, 63, // k=217, Θ=216.0°
    29, 61, // k=218, Θ=217.0°
    31, 58, // k=219, Θ=218.0°
    32, 56, // k=220, Θ=219.0°
    34, 54, // k=221, Θ=220.0°
    35, 52, // k=222, Θ=221.0°
    37, 50, // k=223, Θ=222.0°
    39, 48, // k=224, Θ=223.0°
    40, 46, // k=225, Θ=224.0°
    42, 45, // k=226, Θ=225.0°
    44, 43, // k=227, Θ=226.0°
    46, 41, // k=228, Θ=227.0°
    48, 39, // k=229, Θ=228.0°
    50, 37, // k=230, Θ=229.0°
    51, 36, // k=231, Θ=230.0°
    53, 34, // k=232, Θ=231.0°
    55, 32, // k=233, Θ=232.0°
    57, 31, // k=234, Θ=233.0°
    59, 29, // k=235, Θ=234.0°
    61, 27, // k=236, Θ=235.0°
    63, 26, // k=237, Θ=236.0°
    66, 25, // k=238, Θ=237.0°
    68, 23, // k=239, Θ=238.0°
    70, 22, // k=240, Θ=239.0°
    72, 20, // k=241, Θ=240.0°
    74, 19, // k=242, Θ=241.0°
    76, 18, // k=243, Θ=242.0°
    79, 17, // k=244, Θ=243.0°
    81, 15, // k=245, Θ=244.0°
    83, 14, // k=246, Θ=245.0°
    85, 13, // k=247, Θ=246.0°
    88, 12, // k=248, Θ=247.0°
    90, 11, // k=249, Θ=248.0°
    92, 10, // k=250, Θ=249.0°
    95, 9, // k=251, Θ=250.0°
    97, 8, // k=252, Θ=251.0°
    100, 7, // k=253, Θ=252.0°
    102, 7, // k=254, Θ=253.0°
    104, 6, // k=255, Θ=254.0°
    107, 5, // k=256, Θ=255.0°
    109, 5, // k=257, Θ=256.0°
    112, 4, // k=258, Θ=257.0°
    114, 3, // k=259, Θ=258.0°
    117, 3, // k=260, Θ=259.0°
    119, 2, // k=261, Θ=260.0°
    121, 2, // k=262, Θ=261.0°
    124, 1, // k=263, Θ=262.0°
    126, 1, // k=264, Θ=263.0°
    129, 1, // k=265, Θ=264.0°
    131, 1, // k=266, Θ=265.0°
    134, 0, // k=267, Θ=266.0°
    136, 0, // k=268, Θ=267.0°
    139, 0, // k=269, Θ=268.0°
    141, 0, // k=270, Θ=269.0°
    144, 0, // k=271, Θ=270.0°
    147, 0, // k=272, Θ=271.0°
    149, 0, // k=273, Θ=272.0°
    152, 0, // k=274, Θ=273.0°
    154, 0, // k=275, Θ=274.0°
    157, 1, // k=276, Θ=275.0°
    159, 1, // k=277, Θ=276.0°
    162, 1, // k=278, Θ=277.0°
    164, 1, // k=279, Θ=278.0°
    167, 2, // k=280, Θ=279.0°
    169, 2, // k=281, Θ=280.0°
    171, 3, // k=282, Θ=281.0°
    174, 3, // k=283, Θ=282.0°
    176, 4, // k=284, Θ=283.0°
    179, 5, // k=285, Θ=284.0°
    181, 5, // k=286, Θ=285.0°
    184, 6, // k=287, Θ=286.0°
    186, 7, // k=288, Θ=287.0°
    188, 7, // k=289, Θ=288.0°
    191, 8, // k=290, Θ=289.0°
    193, 9, // k=291, Θ=290.0°
    196, 10, // k=292, Θ=291.0°
    198, 11, // k=293, Θ=292.0°
    200, 12, // k=294, Θ=293.0°
    203, 13, // k=295, Θ=294.0°
    205, 14, // k=296, Θ=295.0°
    207, 15, // k=297, Θ=296.0°
    209, 17, // k=298, Θ=297.0°
    212, 18, // k=299, Θ=298.0°
    214, 19, // k=300, Θ=299.0°
    216, 20, // k=301, Θ=300.0°
    218, 22, // k=302, Θ=301.0°
    220, 23, // k=303, Θ=302.0°
    222, 25, // k=304, Θ=303.0°
    225, 26, // k=305, Θ=304.0°
    227, 27, // k=306, Θ=305.0°
    229, 29, // k=307, Θ=306.0°
    231, 31, // k=308, Θ=307.0°
    233, 32, // k=309, Θ=308.0°
    235, 34, // k=310, Θ=309.0°
    237, 36, // k=311, Θ=310.0°
    238, 37, // k=312, Θ=311.0°
    240, 39, // k=313, Θ=312.0°
    242, 41, // k=314, Θ=313.0°
    244, 43, // k=315, Θ=314.0°
    246, 45, // k=316, Θ=315.0°
    248, 46, // k=317, Θ=316.0°
    249, 48, // k=318, Θ=317.0°
    251, 50, // k=319, Θ=318.0°
    253, 52, // k=320, Θ=319.0°
    254, 54, // k=321, Θ=320.0°
    256, 56, // k=322, Θ=321.0°
    257, 58, // k=323, Θ=322.0°
    259, 61, // k=324, Θ=323.0°
    260, 63, // k=325, Θ=324.0°
    262, 65, // k=326, Θ=325.0°
    263, 67, // k=327, Θ=326.0°
    265, 69, // k=328, Θ=327.0°
    266, 71, // k=329, Θ=328.0°
    267, 74, // k=330, Θ=329.0°
    269, 76, // k=331, Θ=330.0°
    270, 78, // k=332, Θ=331.0°
    271, 81, // k=333, Θ=332.0°
    272, 83, // k=334, Θ=333.0°
    273, 85, // k=335, Θ=334.0°
    275, 88, // k=336, Θ=335.0°
    276, 90, // k=337, Θ=336.0°
    277, 93, // k=338, Θ=337.0°
    278, 95, // k=339, Θ=338.0°
    278, 98, // k=340, Θ=339.0°
    279, 100, // k=341, Θ=340.0°
    280, 103, // k=342, Θ=341.0°
    281, 105, // k=343, Θ=342.0°
    282, 108, // k=344, Θ=343.0°
    282, 110, // k=345, Θ=344.0°
    283, 113, // k=346, Θ=345.0°
    284, 115, // k=347, Θ=346.0°
    284, 118, // k=348, Θ=347.0°
    285, 120, // k=349, Θ=348.0°
    285, 123, // k=350, Θ=349.0°
    286, 126, // k=351, Θ=350.0°
    286, 128, // k=352, Θ=351.0°
    287, 131, // k=353, Θ=352.0°
    287, 133, // k=354, Θ=353.0°
    287, 136, // k=355, Θ=354.0°
    287, 139, // k=356, Θ=355.0°
    288, 141, // k=357, Θ=356.0°
    288, 144, // k=358, Θ=357.0°
    288, 147, // k=359, Θ=358.0°
    288, 149, // k=360, Θ=359.0°
};
