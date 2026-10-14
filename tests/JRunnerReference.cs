using System; using System.IO; using System.Diagnostics;
class ISD_NAND { public int count; public byte[] isd_bin=new byte[0xB000],startup,eject,header,table;
private byte[] split_nible(int num)
            {
                byte[] bytes = new byte[2];
                bytes[1] = (byte)(num >> 8);
                bytes[0] = (byte)num;
                return bytes;
            }
public byte[] wav_to_raw(byte[] wav)
            {
                byte[] raw = new byte[wav.Length];
                byte[] magic = { 0x64, 0x61, 0x74, 0x61 };
                byte[] length = new byte[4];
                int i = 0;
                int got = 0;
                int offset = 0;
                bool data = false;
                for (int counter = 0; counter < wav.Length; counter++)
                {
                    if (!data)
                    {
                        if (got >= 3)
                        {
                            data = true;
                            length[0] = wav[counter + 4];
                            length[1] = wav[counter + 3];
                            length[2] = wav[counter + 2];
                            length[3] = wav[counter + 1];
                            counter += 4;
                            offset = counter;
                        }
                        else
                        {
                            if (wav[counter] == magic[got]) got++;
                            else got = 0;
                        }
                    }
                    else
                    {
                        if (counter >= bytearray_to_int(length) + offset)
                        {
                            break;
                        }
                        if ((i % 0xFFB) == 0)
                        {
                            Debug.WriteLine("{0:X}", i);
                            raw[i] = 0x78;
                            i++;
                        }
                        counter++;
                        raw[i] = wav[counter];
                        i++;
                    }
                }
                byte[] temp = new byte[i];
                Buffer.BlockCopy(raw, 0, temp, 0, i);
                raw = temp;

                return raw;
            }
public static int bytearray_to_int(byte[] value)
            {
                return Convert.ToInt32(ByteArrayToString(value), 16);
            }
public static string ByteArrayToString(byte[] ba, int startindex = 0, int length = 0)
            {
                if (ba == null) return "";
                string hex = BitConverter.ToString(ba);
                if (startindex == 0 && length == 0) hex = BitConverter.ToString(ba);
                else if (length == 0 && startindex != 0) hex = BitConverter.ToString(ba, startindex);
                else if (length != 0 && startindex != 0) hex = BitConverter.ToString(ba, startindex, length);
                return hex.Replace("-", "");
            }
public byte[] generate_isd_bin(byte[] startup, byte[] eject)
            {
                ISD_NAND bin = new ISD_NAND();

                byte[] header = {
                                       0xCF, 0xB, 0x0, 0x0, 0x0, 0x59, 0x8A, 0x0, 0x7D,
                                       0x8A, 0x0, 0x7E, 0x8A, 0x0, 0x9C, 0x8A, 0x0, 0x9D,
                                       0x8A, 0x0, 0xBB, 0x8A, 0x0, 0xBC, 0x8A, 0x0, 0xC5,
                                       0x8A, 0x0, 0xC6, 0x8A, 0x0, 0xCF, 0x8A, 0x0, 0x3E,
                                       0x0, 0x0, 0x4C, 0x7A, 0x0, 0x4D, 0x7A, 0x0, 0x58,
                                       0x8A, 0x0, 0x0, 0xFF, 0xFF, 0x6B, 0x3B, 0x66, 0x44,
                                       0x62, 0x62, 0x3B, 0x1E, 0x92, 0xC3, 0x4C, 0x0
                                   };
                byte[] table = {
                                      0xB8, 0x1A, 0x0, 0xB8, 0x1B, 0x0, 0xB8, 0x19, 0x40, 0xB8,
                                      0x1D, 0x0, 0xB8, 0x1E, 0x28, 0xB8, 0x1F, 0x0, 0xB8, 0x0,
                                      0x64, 0xB8, 0x1, 0x20, 0xB8, 0x2, 0x44, 0xB8, 0x26, 0x3,
                                      0xB8, 0x3, 0x2, 0xB8, 0x2A, 0x4, 0x12, 0xB8, 0x1A, 0x0, 0xB8,
                                      0x1B, 0x0, 0xB8, 0x1E, 0x28, 0xB8, 0x1F, 0x0, 0xB8, 0x26, 0x3,
                                      0xB8, 0x2A, 0x4, 0xB8, 0x1, 0x20, 0xB8, 0x3, 0x2, 0xB8, 0x0,
                                      0x64, 0xB8, 0x2, 0x44, 0xFF, 0xB8, 0x1A, 0x0, 0xB8, 0x19, 0x0,
                                      0xB8, 0x1B, 0x0, 0xB8, 0x1E, 0x28, 0xB8, 0x1F, 0x0, 0xB8, 0x26,
                                      0x3, 0xB8, 0x2A, 0x4, 0xB8, 0x1, 0x40, 0xB8, 0x2, 0x44, 0xB8,
                                      0x3, 0x2, 0xFF, 0xB8, 0x3, 0x16, 0xA6, 0x0, 0x6, 0xFC, 0x0, 0x0,
                                      0x12, 0xB8, 0x3, 0x0, 0xA6, 0x0, 0x5, 0xFC, 0x0, 0x0, 0x12, 0x49,
                                      0x53, 0x44, 0x2D, 0x56, 0x50, 0x45, 0x20, 0x56, 0x65, 0x72, 0x20,
                                      0x32, 0x31, 0x30, 0x2E, 0x30, 0x30, 0x34, 0x35, 0x20, 0x30, 0x35,
                                      0x2F, 0x31, 0x31, 0x2F, 0x32, 0x30, 0x31, 0x31, 0x20, 0x20
                                  };
                bin.header = header;
                bin.table = table;
                bin.startup = startup;
                bin.eject = eject;
                bin.count = startup.Length + eject.Length;

                //Dim dato(1) As Integer
                byte[] dato = new byte[2];
                //'Form1.TextBox3.Text = Form1.TextBox3.Text & "Start User Sound at Offset 0x3E" & vbCrLf
                dato = split_nible(bin.count + 0x3D);
                //'Form1.TextBox3.Text = Form1.TextBox3.Text & "End User Sound at Offset 0x" & Convert.ToString(dato(1), 16) & Convert.ToString(dato(0), 16) & vbCrLf
                dato = split_nible(bin.count + 0x3E);

                //'-----EntryPoint TablePOI
                //'Form1.TextBox3.Text = Form1.TextBox3.Text & "Start EP TablePOI Offset 0x" & Convert.ToString(dato(1), 16) & Convert.ToString(dato(0), 16) & vbCrLf
                bin.header[5] = dato[0];
                bin.header[6] = dato[1];
                dato = split_nible(bin.count + 0x3E + 0x24);
                bin.header[8] = dato[0];
                bin.header[9] = dato[1];
                //'Form1.TextBox3.Text = Form1.TextBox3.Text & "End EP TablePOI Offset 0x" & Convert.ToString(dato(1), 16) & Convert.ToString(dato(0), 16) & vbCrLf
                //'--------------------------

                //'-----EntryPoint TablePU
                dato = split_nible(bin.count + 0x3E + 0x25);
                //'Form1.TextBox3.Text = Form1.TextBox3.Text & "Start EP TablePU Offset 0x" & Convert.ToString(dato(1), 16) & Convert.ToString(dato(0), 16) & vbCrLf
                bin.header[0xB] = dato[0];
                bin.header[0xC] = dato[1];
                dato = split_nible(bin.count + 0x3E + 0x43);
                bin.header[0xE] = dato[0];
                bin.header[0xF] = dato[1];
                //'Form1.TextBox3.Text = Form1.TextBox3.Text & "End EP TablePU Offset 0x" & Convert.ToString(dato(1), 16) & Convert.ToString(dato(0), 16) & vbCrLf
                //'--------------------------

                //'-----EntryPoint GPIO
                dato = split_nible(bin.count + 0x3E + 0x44);
                //'Form1.TextBox3.Text = Form1.TextBox3.Text & "Start EP TableGPIO Offset 0x" & Convert.ToString(dato(1), 16) & Convert.ToString(dato(0), 16) & vbCrLf
                bin.header[0x11] = dato[0];
                bin.header[0x12] = dato[1];
                dato = split_nible(bin.count + 0x3E + 0x62);
                bin.header[0x14] = dato[0];
                bin.header[0x15] = dato[1];
                //'Form1.TextBox3.Text = Form1.TextBox3.Text & "End EP TableGPIO Offset 0x" & Convert.ToString(dato(1), 16) & Convert.ToString(dato(0), 16) & vbCrLf
                //'--------------------------

                //'-----EntryPoint VM SCRIPT
                dato = split_nible(bin.count + 0x3E + 0x63);
                //'Form1.TextBox3.Text = Form1.TextBox3.Text & "Start EP TableVMScript Offset 0x" & Convert.ToString(dato(1), 16) & Convert.ToString(dato(0), 16) & vbCrLf
                bin.header[0x17] = dato[0];
                bin.header[0x18] = dato[1];
                dato = split_nible(bin.count + 0x3E + 0x6C);
                bin.header[0x1A] = dato[0];
                bin.header[0x1B] = dato[1];
                //'Form1.TextBox3.Text = Form1.TextBox3.Text & "End EP TableVMScript Offset 0x" & Convert.ToString(dato(1), 16) & Convert.ToString(dato(0), 16) & vbCrLf
                //'--------------------------

                //'-----EntryPoint MACRO SCRIPT
                dato = split_nible(bin.count + 0x3E + 0x6D);
                //'Form1.TextBox3.Text = Form1.TextBox3.Text & "Start EP TableMacroScript Offset 0x" & Convert.ToString(dato(1), 16) & Convert.ToString(dato(0), 16) & vbCrLf
                bin.header[0x1D] = dato[0];
                bin.header[0x1E] = dato[1];
                dato = split_nible(bin.count + 0x3E + 0x76);
                bin.header[0x20] = dato[0];
                bin.header[0x21] = dato[1];
                //'Form1.TextBox3.Text = Form1.TextBox3.Text & "End EP TableMacroScript Offset 0x" & Convert.ToString(dato(1), 16) & Convert.ToString(dato(0), 16) & vbCrLf
                //'--------------------------

                //'-----Startup Sound Pointer
                //Form1.TextBox3.Text = Form1.TextBox3.Text & "Startup Sound Start Offset 0x" & Convert.ToString(0x3E, 16) & vbCrLf
                bin.header[0x23] = 0x3E;
                //bin.header(0x24) = 0
                dato = split_nible(bin.startup.Length - 1 + 0x3E);
                bin.header[0x26] = dato[0];
                bin.header[0x27] = dato[1];
                //Form1.TextBox3.Text = Form1.TextBox3.Text & "Startup Sound End Offset 0x" & Convert.ToString(dato(1), 16) & Convert.ToString(dato(0), 16) & vbCrLf
                //'--------------------------

                //'-----Eject Sound Pointer
                dato = split_nible(bin.startup.Length + 0x3E);
                //Form1.TextBox3.Text = Form1.TextBox3.Text & "Eject Sound Start Offset 0x" & Convert.ToString(dato(1), 16) & Convert.ToString(dato(0), 16) & vbCrLf
                bin.header[0x29] = dato[0];
                bin.header[0x2A] = dato[1];
                dato = split_nible(bin.startup.Length + bin.eject.Length - 1 + 0x3E);
                bin.header[0x2C] = dato[0];
                bin.header[0x2D] = dato[1];
                //Form1.TextBox3.Text = Form1.TextBox3.Text & "Eject Sound End Offset 0x" & Convert.ToString(dato(1), 16) & Convert.ToString(dato(0), 16) & vbCrLf
                //'--------------------------


                int i = 0;

                Buffer.BlockCopy(bin.header, 0, bin.isd_bin, i, bin.header.Length);
                i += bin.header.Length;

                Buffer.BlockCopy(bin.startup, 0, bin.isd_bin, i, bin.startup.Length);
                i += bin.startup.Length;

                Buffer.BlockCopy(bin.eject, 0, bin.isd_bin, i, bin.eject.Length);
                i += bin.eject.Length;

                Buffer.BlockCopy(bin.table, 0, bin.isd_bin, i, bin.table.Length);
                i += bin.table.Length;

                for (; i < bin.isd_bin.Length; i++)
                {
                    bin.isd_bin[i] = 0xFF;
                }

                return bin.isd_bin;
            }
}
class Reference { static void Main(string[] args) { var n=new ISD_NAND(); File.WriteAllBytes(args[2],n.generate_isd_bin(n.wav_to_raw(File.ReadAllBytes(args[0])),n.wav_to_raw(File.ReadAllBytes(args[1])))); } }
