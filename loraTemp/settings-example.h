/*
  If you have Python available, you can use this 2-liner to produce keys:

line = '0123456789abcdef0123456789abcdef'
print(', '.join(['0x'+line[i:i+2] for i in range(0, len(line), 2)]))
-->
0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef

*/

// ABP
// USE YOUR OWN KEYS!

const uint8_t devAddr[4] =
{
  // 1234abcd
  0x12, 0x34, 0xab, 0xcd
};

// USE YOUR OWN KEYS!
const uint8_t nwkSKey[16] =
{
  // 0123456789abcdef0123456789abcdef
  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
};

// USE YOUR OWN KEYS!
const uint8_t appSKey[16] =
{
  // fedcba9876543210fedcba9876543210
  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
};


/********** Values below should be left empty **********/

// OTAA
// With using the GetHWEUI() function the HWEUI will be used
static uint8_t DevEUI[8]
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t AppEUI[8] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t AppKey[16] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
