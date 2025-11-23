// #include "out.h"



// void __NVIC_EnableIRQ(ushort param_1)

// {
//   if (-1 < (short)param_1) {
//     *(int *)(((uint)(int)(short)param_1 >> 5) * 4 + -0x1fff1f00) = 1 << (param_1 & 0x1f);
//   }
//   return;
// }



// // WARNING: Globals starting with '_' overlap smaller symbols at the same address

// void spiActivate(char param_1,char param_2,char param_3,char param_4)

// {
//   uint uVar1;
//   uint uVar2;
//   undefined1 local_19;
  
//   if (param_3 == '\0') {
//     uVar1 = 0;
//   }
//   else {
//     uVar1 = 0x20000000;
//   }
//   if (param_4 == '\0') {
//     uVar2 = 0x10000000;
//   }
//   else {
//     uVar2 = 0;
//   }
//   if (param_2 == '\0') {
//     _DAT_40003404 = 0;
//   }
//   else {
//     _DAT_40003404 = 0x20000;
//   }
//   local_19 = param_1;
//   if (param_1 == '\0') {
//     local_19 = '\x01';
//   }
//   do {
//   } while (_DAT_4000341c != 0);
//   DAT_4000340c = local_19;
//   _DAT_40003400 = uVar2 | uVar1 | 0x30000e;
//   return;
// }



// // WARNING: Globals starting with '_' overlap smaller symbols at the same address

// void spiDeactivate(void)

// {
//   _DAT_40003400 = 0;
//   _DAT_40003404 = 0;
//   do {
//   } while (_DAT_4000341c != 0);
//   return;
// }



// // WARNING: Globals starting with '_' overlap smaller symbols at the same address

// void spiInit(void)

// {
//   __NVIC_EnableIRQ(0x32);
//   _DAT_40001c2c = 0x10106;
//   do {
//   } while ((_DAT_40001c04 & 0x20) == 0x20);
//   _DAT_40001ca0 = 0x43;
//   _DAT_40000814 = _DAT_40000814 | 0x2000;
//   DAT_41008050 = DAT_41008050 | 1;
//   DAT_41008051 = DAT_41008051 | 1;
//   DAT_41008038 = DAT_41008038 | 0x22;
//   DAT_41008053 = DAT_41008053 | 3;
//   DAT_41008039 = DAT_41008039 | 0x20;
//   return;
// }



// // WARNING: Removing unreachable block (ram,0x00010278)

// void spiWriteByte(void)

// {
//                     // WARNING: Do nothing block with infinite loop
//   do {
//   } while( true );
// }



// // WARNING: Globals starting with '_' overlap smaller symbols at the same address

// void displayCmd(undefined1 param_1)

// {
//   _DAT_41008094 = 0x200;
//   spiWriteByte(param_1);
//   _DAT_41008098 = 0x200;
//   return;
// }



// // WARNING: Globals starting with '_' overlap smaller symbols at the same address

// void activate(void)

// {
//   spiActivate(1,0,0,1);
//   _DAT_41008094 = 0x10;
//   return;
// }



// // WARNING: Globals starting with '_' overlap smaller symbols at the same address

// void deactivate(void)

// {
//   _DAT_41008098 = 0x10;
//   spiDeactivate();
//   return;
// }



// void displayWake(void)

// {
//   activate();
//   displayCmd(0xaf);
//   deactivate();
//   return;
// }



// void displayDrawDigit(byte param_1,char param_2,undefined2 param_3,byte param_4)

// {
//   int local_1c;
//   uint local_18;
//   undefined2 local_14;
//   char local_11;
  
//   if (param_4 < 10) {
//     activate();
//     for (local_18 = 0; (int)local_18 < 0xc; local_18 = local_18 + 1) {
//       displayCmd(0x15);
//       spiWriteByte(param_2 + '\x10');
//       spiWriteByte(param_2 + '\x18');
//       displayCmd(0x75);
//       spiWriteByte(((uint)param_1 - (local_18 & 0xff) & 0xff) + 0xc & 0xff);
//       spiWriteByte(((uint)param_1 - (local_18 & 0xff) & 0xff) + 0xc & 0xff);
//       displayCmd(0x5c);
//       local_11 = (&fontTable)[local_18 + (uint)param_4 * 0xc];
//       for (local_1c = 0; local_1c < 8; local_1c = local_1c + 1) {
//         local_14 = param_3;
//         if (-1 < local_11) {
//           local_14 = 0;
//         }
//         spiWriteByte((char)((ushort)local_14 >> 8));
//         spiWriteByte((char)local_14);
//         local_11 = local_11 << 1;
//       }
//     }
//     deactivate();
//   }
//   return;
// }



// void displayDrawPixel(char param_1,undefined1 param_2,undefined2 param_3)

// {
//   activate();
//   displayCmd(0x15);
//   spiWriteByte(param_1 + '\x10');
//   spiWriteByte(param_1 + '\x10');
//   displayCmd(0x75);
//   spiWriteByte(param_2);
//   spiWriteByte(param_2);
//   displayCmd(0x5c);
//   spiWriteByte((char)((ushort)param_3 >> 8));
//   spiWriteByte((char)param_3);
//   deactivate();
//   return;
// }



// void displayReplacePixel(undefined1 param_1,undefined1 param_2,undefined2 param_3)

// {
//   if (firstPixel_2 == '\x01') {
//     firstPixel_2 = '\0';
//   }
//   else {
//     displayDrawPixel(prevX_1,prevY_0,0);
//   }
//   displayDrawPixel(param_1,param_2,param_3);
//   prevX_1 = param_1;
//   prevY_0 = param_2;
//   return;
// }



// void displayErase(void)

// {
//   undefined4 local_10;
//   undefined4 local_c;
  
//   activate();
//   displayCmd(0x15);//tell display that we are writing to row
//   spiWriteByte(0);//from this to
//   spiWriteByte(0x7f);//this
//   displayCmd(0x75);//tell display that we are writing to col
//   spiWriteByte(0);// from this to
//   spiWriteByte(0x7f);// this
//   displayCmd(0x5c);//write to ram
//   for (local_c = 0; local_c < 0x80; local_c = local_c + 1) {
//     for (local_10 = 0; local_10 < 0x80; local_10 = local_10 + 1) {
//       spiWriteByte(0);
//       spiWriteByte(0);
//     }
//   }
//   deactivate();
//   return;
// }



// // WARNING: Globals starting with '_' overlap smaller symbols at the same address

// void displayInit(void)

// {
//   undefined4 local_c;
  
//   spiInit();
//   _DAT_41008088 = 0x200;
//   _DAT_41008094 = 0x100;
//   _DAT_41008098 = 0x80;
//   displayWake();
//   activate();
//   displayCmd(0xa0);
//   spiWriteByte(0x32);
//   displayCmd(0xa1);
//   spiWriteByte(0x20);
//   displayCmd(0xb2);
//   spiWriteByte(0xa4);
//   spiWriteByte(0);
//   spiWriteByte(0);
//   displayErase();
//   deactivate();
//   for (local_c = 0; local_c < 1000000; local_c = local_c + 1) {
//   }
//   return;
// }



