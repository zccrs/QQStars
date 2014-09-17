#ifndef AES_H
#define AES_H

#include <string.h>

class AES
{
public:
  AES(char* key);
  virtual ~AES();
  char* Cipher(char* input);
  char* InvCipher(char* input);
  void* Cipher(void* input, int length=0);
  void* InvCipher(void* input, int length);

  void Cipher(char *input, char *output);
  void InvCipher(char *inut, char *output);

private:
  char Sbox[256];
  char InvSbox[256];
  char w[11][4][4];

  void KeyExpansion(char* key, char w[][4][4]);
  char FFmul(char a, char b);

  void SubBytes(char state[][4]);
  void ShiftRows(char state[][4]);
  void MixColumns(char state[][4]);
  void AddRoundKey(char state[][4], char k[][4]);

  void InvSubBytes(char state[][4]);
  void InvShiftRows(char state[][4]);
  void InvMixColumns(char state[][4]);

  int strToHex(const char *ch, char *hex);
  int hexToStr(const char *hex, char *ch);
  int ascillToValue(const char ch);
  char valueToHexCh(const int value);
  int getUCharLen(const char *uch);
  int strToUChar(const char *ch, char *uch);
  int ucharToStr(const char *uch, char *ch);
  int ucharToHex(const char *uch, char *hex);
  int hexToUChar(const char *hex, char *uch);
};

#endif // AES_H
