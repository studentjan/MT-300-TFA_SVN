using System;
using System.Collections.Generic;

using System.Linq;
using System.Security.AccessControl;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;
using System.Diagnostics;
using System.Timers;

namespace MT_300_TFA_application
{
    public class serial_com
    {
        public Cord_form Cord_object;
        public Thread task1;
        public Thread task2;
        public Thread task3;
        //++++++++++++++++++++++++++++++++++++++++++++++NASTAVITVE+++++++++++++++++++++++++++++++++++++++++++++++++++++++
        private const int QUEUE_COMMAND_BUFFER_SIZE = 300;
            //velikost char[] bufferja v katerega se shrani prispela komanda pred parsanjem sporocila

        private const int QUEUE_BUFFER_SIZE = 10; //velikost cakalne vrste sporocila
        private const int TRANSMIT_HANDLE_BUFF_SIZE = 10; //velikost bufferja za shranjevanje oddanih komand
        private const int NUM_NACK_EVENTS = 3; //stevilo ponovitev posiljanja oddane komande
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        //druge konstante: ne spreminjaj
        private const int QUEUE_FREE = 0;
        private const int QUEUE_INDEX_MAX = 300;
        private const int QUEUE_FAIL = 0;
        private const int QUEUE_PASS = 1;
        private const int NUMBER_OF_DELIMITERS = 8; //stevilo delimiterjev(:) v komandi
        private int[] delimiterArray = new int[NUMBER_OF_DELIMITERS + 1];
        private const int MAX_ADDITIONAL_COMMANDS = 10;
        private const int _ANS_MESSAGE_NACK = 1;
        private const int _ANS_MESSAGE_ACK = 0;
        private const int INPUT_BUFFER_LENGTH = 300;
        private const char UART_COMMAND_CHARACTER_13 = (char) 13;
        private const char UART_COMMAND_CHARACTER_10 = (char) 10;
        public const int COMM_DIR_PORT1 = 1;
        public const int COMM_DIR_PORT2 = 2;
        public const int COMM_DIR_PORT3 = 3;
        private bool task1_ex_flag;
        private bool task2_ex_flag;
        private int counter1 = 0;
        private int counter2 = 0;
        private int counter3 = 0;
        private int counter4 = 0;
        private int counter5 = 0;
        private int counter6 = 0;

        //spremenljivke
        private int ID;
        private char transmitter_ID;
        private char reciever_ID;
        public string m_start_tag;
        public string m_msg_ID;
        public string m_function;
        public string m_command;
        public string m_additional_code;
        public string m_value;
        public string m_is_crc;
        public string m_crc_value;
        public string m_leftover;

        public string[,] additionalCode = new string[2, MAX_ADDITIONAL_COMMANDS];
            //[MAX_ADDITIONAL_COMMANDS_LENGTH]; //0 so funkcije 1 pa vrednosti

        private int queue_counter; //counter, ki steje kok polhn je polje
        private int queue_temp;
        private int queue_temp1;
        private int queue_RxBuffer_index;
        private char[] input_buffer = new char[300];
        private int message_to_send = 0;

        private int event_status;
        private int send_id_count = 1;
        private int transmitt_handle_write_count = 0;
        private int transmitt_handle_read_count = 0;

        //cakalne vrste
        private Queue<transmittOutBuffer> TransmittSynchroQueue = new Queue<transmittOutBuffer>();

        //timerji
        public System.Timers.Timer synchroTimer;
       

        //+++++++++++++++++++++++++++++++++++++++++KOMANDE ZA PREJEMANJE IN POSILJANJE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        private const int MAX_VALID_COMMANDS = 50;
        //                                                  0           1           2           3           4           5           6           7           8           9           10          11          12          13          14          15          16          17          18          19          20          21          22          23          24          25          26          27          28          29          30          31          32          33          34          35          36       
        public static String[] FUNCTION_COMMUNICATON_NAMES = {"MT-300-TFA"};
            //                                                   0          1       2       3           4       5       6        7       8
        public static String[] COMMAND_TYPE_NAMES =  {        "POWER", "RELAY", "WARNING", "TEST", "COMMUN", "CORD", "STATUS" ,"MACH", "WELD" };

        public static String[] CONNECTION_CODE_NAMES = {"CONNECT_REQUEST", "CONNECTION_ESTABLED", "CONNECTION_CHECK"};
        public static String[] CONNECTION_ADD_NAMES = {"MT-310", "OK", "NOK"};
        public static String[] TEST_CODE_NAMES = {"PROTOCOL_TEST"};
             //                                           0          1      2               3           4       5           6                   7                    8                      9               10          11          12           13         14              15              16              17              18          19              20              21              22          23              24          25          26              27      28          29          30          31
        public static String[] CORD_CODE_NAMES = { "START_NORMAL", "STOP", "INIT", "RPE_RES_GET", "RPE_RES", "STOPPED", "INITIATED", "RISO_ALL-PE_RESULT", "RISO_ONE-PE_RESULT", "RISO_PH-PH_RESULT", "START_C_W", "CW_RESULT", "START_CONT", "CONT", "START_ALL-PE", "START_ONE-PE", "START_PH_PH", "RISO_RES_GET", "RISO_RES", "RPE_L_START", "RPE_L_STARTED", "RPE_H_START","RPE_H_STARTED","RISO_START","RISO_STARTED","RPE_STOP","RPE_STOPPED","RISO_STOP","RISO_STOPPED","RES","DEINITIATED","DEINIT"};
        public static String[] CORD_LEFTOVER_NAMES = { "L1_L1", "L1_L2", "L1_L3", "L1_N", "L1_PE", "L2_L1", "L2_L2", "L2_L3", "L2_N", "L2_PE",
                                                        "L3_L1","L3_L2", "L3_L3", "L3_N", "L3_PE", "N_L1" , "N_L2" , "N_L3" , "N_N" , "N_PE" ,
                                                        "PE_L1", "PE_L2", "PE_L3", "PE_N","PE_PE", "1_P","3_P"};
        //                                                          0           1           2        3      4       5       6           7           8         9     10      11      12      13          14      15
        public static String[] CORD_LEFTOVER_RISO_NAMES = { "L1-L2-L3-N_PE","L2-L3-N_PE","L3-N_PE","N_PE","L3_PE", "L2_PE", "L1_PE","L1-L2-L3_N","L2-L3_N","L3_N","L2_N","L1_N", "L1-L2_L3","L2_L3", "L1_L3","L1_L2"};
        //                                            0           1       2       3       4           5       6       7          8
        public static String[] URES_PARAM_NAMES = { "L1-PE" , "L2-PE", "L3-PE", "L1-N" , "L2-N" , "L3-N" , "L1-L2" , "L2-L3" ,"L1-L3" };
        //                                                  0           1       2           3               4               5            6           7           8                9         10        11          12           13          14               15              16                  17                  18                  19              20           21             22          23              24
        public static String[] MACH_COMMAND_NAMES = { "START_URES", "INIT", "DEINIT", "URES_STARTED", "URES_STOPPED", "URES_FINISHED", "STOP", "URES_OPEN", "URES_OPENED", "INITIATED", "TEST", "START_RPE", "STOP_RPE", "RPE_STARTED", "RPE_STOPPED", "START_ALL-PE", "START_ONE-PE", "RISO_ALL-PE_RESULT", "RISO_ONE-PE_RESULT", "RISO_START", "RISO_STARTED", "RISO_STOP", "RISO_STOPPED", "RISO_RES_GET", "RISO_RES" };
        public static String[] MACH_ADD_NAMES = { "TEST" };
        public static String[] STATUS_CODE_NAMES = {""};
        public static String[] STATUS_VALUE_NAMES = {""};
        public static String[] WARNING_CODE_NAMES = {"COMMAND_SEND_ERROR"};
        public static String[] POWER_CODE_NAMES = {"START", "STOP", "START_NTHD",};
        //                                                  0           1       2           3               4               5            6           7           8                9         10        11          12           13          14               15              16                  17                  18                  19              20           21             22          23              24                  25                  26                  27              28                      29                  30                  31              32              33              34
        public static String[] WELD_COMMAND_NAMES = { "START_URES", "INIT", "DEINIT", "URES_STARTED", "URES_STOPPED", "URES_FINISHED", "STOP", "URES_OPEN", "URES_OPENED", "INITIATED", "TEST", "START_RPE", "STOP_RPE", "RPE_STARTED", "RPE_STOPPED", "START_ALL-PE", "START_ONE-PE", "RISO_ALL-PE_RESULT", "RISO_ONE-PE_RESULT", "RISO_START", "RISO_STARTED", "RISO_STOP", "RISO_STOPPED", "RISO_RES_GET", "RISO_RES", "START_MAINS-WELD", "START_MAINS-CLASS2", "START_WELD-PE", "MAINS-WELD_RESULT", "MAINS-CLASS2_RESULT", "WELD-PE_RESULT", "START_UNL_RMS", "STOP_UNL_RMS", "START_UNL_PEAK", "STOP_UNL_PEAK" };
        //                                             0       1
        public static String[] WELD_CODE_NAMES = { "SINGLE", "CONT"};
        public static String[] RELAY_CODE_NAMES =
        {
            "RESET_ALL", "1_38_ON", "1_38_OFF", "2_ON", "2_OFF", "3_ON", "3_OFF", "4_ON", "4_OFF", "5_ON",
            "5_OFF", "6_ON", "6_OFF", "7_ON", "7_OFF", "8_ON", "8_OFF", "9_ON", "9_OFF", "10_ON",
            "10_OFF", "11_ON", "11_OFF", "12_ON", "12_OFF", "13_ON", "13_OFF", "14_ON", "14_OFF", "15_ON",
            "15_OFF", "16_ON", "16_OFF", "17_ON", "17_OFF", "18_ON", "18_OFF", "19_ON", "19_OFF", "20_ON",
            "20_OFF", "21_43_ON", "21_43_OFF", "22_ON", "22_OFF", "23_ON", "23_OFF", "24_ON", "24_OFF", "25_ON",
            "25_OFF", "26_ON", "26_OFF", "27_ON", "27_OFF", "28_ON", "28_OFF", "29_ON", "29_OFF", "30_ON",
            "30_OFF", "31_ON", "31_OFF", "32_ON", "32_OFF", "33_ON", "33_OFF", "34_ON", "34_OFF", "35_ON",
            "35_OFF", "36_ON", "36_OFF", "37_ON", "37_OFF", "1_38_ON", "1_38_OFF", "39_ON", "39_OFF", "40_ON",
            "40_OFF", "41_ON", "41_OFF", "42_ON", "42_OFF", "21_43_ON", "21_43_OFF"
        };


        //+++++++++++++++++++++++++++++++++++++++++++++FUNKCIJE DOSTOPNE OD ZUNAJ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        //inicializacija: poklici pred ostalimi funkcijami v Form1
        public void SerialRx_Init()
        {
            for (int i = 0; i < QUEUE_BUFFER_SIZE; i++)
            {
                command_queue_buffer[i].command_ID = QUEUE_FREE;
                command_queue_buffer[i].command = new char[QUEUE_COMMAND_BUFFER_SIZE];
            }
            setSynchroTimer();
            task1 = new Thread(command_do_events);
            task2 = new Thread(transmit_command_handle);
            event_status = QUEUE_PASS;

        }
        //++++++++++++++++++++++++++++++++++++++++++++++KONSTRUKTORJI RAZREDA++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        //po moznosti dodaj svoje
        //za grajenje objekta v form 1
        private Form1 mainForm; // field to hold the Form1 object reference
        private Cord_form cord_object;
        public serial_com(Form1 form1)
        {
            mainForm = form1;
        }

        public serial_com(Form1 form1, Cord_form form2)
        {
            mainForm = form1;
            cord_object = form2;
        }

        //funkciji vnesemo parametre, ta pa nam sestavi kommando za posiljanje po protokolu
        public int Send_protocol_message(int com_port_dir, char transmitter_ID, char reciever_ID, string function_string, string command_string, string additional_code_str, string data_string)
        {
            int msg_id = send_id_count;
            message_constructor protocol_message = new message_constructor(transmitter_ID, reciever_ID, msg_id, function_string, command_string, additional_code_str, data_string);
            string new_message = ConstructReplyMsg(protocol_message);
            if (Settings1.Default.TRANSMIT_COMMAND_CHECK == 1)//more bit pred posiljanjem sporocila, ker drgac dobimo prej ACK kot se itvede naslednja koda
            {
                char[] message_char = new_message.ToCharArray();
                Transmit_func(new_message.Length, com_port_dir, message_char, protocol_message.msg_ID, protocol_message.reciever);
            }
            serial_send(com_port_dir, new_message);
            //za vsako sporocilo generira svoj id od 1 do 99 
            if (send_id_count >= 99) send_id_count = 1;
            else send_id_count++;
            return msg_id;
        }
        //funkcija, ki nam iz prispelega stringa izloci kommando in jo prestavi v cakalno vrsto za analiziranje.
        //ce imamo komando razbito v vec stringov lahko veckrat poklicemo to funkcijo, ta pa nam ga sestavi in doda v  cakalno vrsto
        public void add_command_to_queue(string Buf, int Len, int dir)
        {
            int queue_cnt;
            int queue_index;
            int cnt;
            int status;
            //temp_str_arr=Buf.Split
            for (cnt = 0; cnt < Len; cnt++)
            {
                if (queue_RxBuffer_index != 0)
                    queue_temp1 = input_buffer[queue_RxBuffer_index - 1];    //predhodna vrednost
                else queue_temp1 = 0;
                input_buffer[queue_RxBuffer_index] = Buf[cnt];
                queue_temp = input_buffer[queue_RxBuffer_index];
                if (++queue_RxBuffer_index >= INPUT_BUFFER_LENGTH)
                {
                    queue_RxBuffer_index = 0;    //overflow -> init pointerja
                    queue_counter = 0;
                }
                else queue_counter++;
            }
            //ko dobi terminatorje skoci tle not
            if ((queue_temp1 == UART_COMMAND_CHARACTER_13) && (queue_temp == UART_COMMAND_CHARACTER_10))
            {
                //nadomesti /13 in /10 z 0
                input_buffer[queue_RxBuffer_index-1] = (char)0;
                input_buffer[queue_RxBuffer_index - 2] = (char)0;
                //poisce kje se komanda zacene. Uporabno v primeru, da je ze kaj drugega prej v bufferju
                for (int i = 0; i < (queue_counter-2); i++)
                {
                    if (input_buffer[i] == '>')
                    {
                        queue_RxBuffer_index = i;//ponastavi indeks na zacetno lokacijo za kopiranje komande
                                                 //ko se funkcija konca spet zacne pisat na zacetk bufferja
                        i = queue_counter;
                        break;
                    }
                    else
                        queue_RxBuffer_index = 0;   //ponastavi indeks na zacetno lokacijo
                                                    //ko se if stavek konca spet zacne pisat na zacetk
                }
                //RxBuffer=&Buf[0];  //init pointerja                                           

                //find command index - (max. index + 1)
                //--------------------------------------          
                queue_index = 0;
                for (queue_cnt = 0; queue_cnt < QUEUE_BUFFER_SIZE; queue_cnt++)
                {
                    if (command_queue_buffer[queue_cnt].command_ID > queue_index) queue_index = command_queue_buffer[queue_cnt].command_ID;
                }
                queue_index += 1;
                //--------------------------------------

                //put command to queue - to first free slot
                //--------------------------------------      
                queue_cnt = 0;
                status = QUEUE_FAIL;
                while ((queue_cnt < QUEUE_BUFFER_SIZE) && (status == QUEUE_FAIL))
                {
                    if (command_queue_buffer[queue_cnt].command_ID == QUEUE_FREE)
                    {
                        command_queue_buffer[queue_cnt].command_ID = queue_index; //index is: max. index before this + 1 
                        command_queue_buffer[queue_cnt].direction = dir; //vpise iz kje je bila prejeta komanda, da ve vrnit ack po istem vodilu
                        for (cnt = 0; cnt < (queue_counter - queue_RxBuffer_index-2); cnt++) //kopira komando v cakalno vrsto
                            command_queue_buffer[queue_cnt].command[cnt] = input_buffer[queue_RxBuffer_index+cnt];
                        for (; cnt < QUEUE_COMMAND_BUFFER_SIZE; cnt++)
                            command_queue_buffer[queue_cnt].command[cnt] = '\0';
                        status = QUEUE_PASS;
                    }
                    queue_cnt++;
                }
                //--------------------------------------

                if ((status == QUEUE_PASS) && (event_status == QUEUE_PASS))
                {
                    if (task1_ex_flag == false)
                    {
                        task1_ex_flag = true;
                        task1 = new Thread(command_do_events);
                        task1.Start();
                        //if (task1.ThreadState == ThreadState.Unstarted) //ce task se ni zagnan ga zazenemo
                        //    task1.Start();
                    }
                    
                }
                queue_counter = 0;
                queue_RxBuffer_index = 0;
                if (Settings1.Default.SYNCHRONUS_TRANSMITT)
                {
                    //resetira timer
                    synchroTimer.Enabled = true;
                    synchroTimer.Stop();
                    synchroTimer.Start();
                    //ko timer pride do konca smo na vrsti za posiljanje mi
                }

            }
        }
        private void OnSynchtoTimedEvent(Object source, ElapsedEventArgs e)
        {
            string temp_str;
            if (TransmittSynchroQueue.Count > 0)
            {
                transmittOutBuffer temp_struct;
                temp_struct = TransmittSynchroQueue.Dequeue();
                temp_str = temp_struct.out_str;
                if (temp_struct.out_dir == Settings1.Default._COMMUNICATION_DIR_PORT1)
                {
                    try
                    {
                        mainForm.serialPort1.Write(temp_str);
                    }
                    catch (System.InvalidOperationException)
                    {
                        MessageBox.Show("COM port closed!", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                    catch (System.IO.IOException)
                    {
                        MessageBox.Show("COM port terminated!", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }

            }
            else
            {
                temp_str = String.Format("{0}{1}{2}::::::0:000:\r\n", Settings1.Default._SER_START_SIGN, Settings1.Default._ID_MT, transmitter_ID);
                if (transmitter_ID == Settings1.Default._ID_TFA)
                {
                    try
                    {
                        mainForm.serialPort1.Write(temp_str);
                    }
                    catch (System.InvalidOperationException)
                    {
                        MessageBox.Show("COM port closed!", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                    catch (System.IO.IOException)
                    {
                        MessageBox.Show("COM port terminated!", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
            mainForm.write_to_send_textbox(temp_str);
        }
        private void setSynchroTimer()
        {
            synchroTimer = new System.Timers.Timer(Settings1.Default.SYNCHRO_SEND_WAIT);
            synchroTimer.AutoReset = false;
            synchroTimer.Elapsed += OnSynchtoTimedEvent;
            synchroTimer.Stop();
        }

        //ce zelimo poslati sporocilo, ki ni po protokolu lahko direktno uporabimo to funkcijo
        public void serial_send(int dir, string send_string)
        {
            transmittOutBuffer temp_buf = new transmittOutBuffer(send_string,dir);
            if (Settings1.Default.SYNCHRONUS_TRANSMITT)
            {
                //dodamo komando v cakalno vrsto
                TransmittSynchroQueue.Enqueue(temp_buf);
            }
            else
            {
                if (dir == Settings1.Default._COMMUNICATION_DIR_PORT1)
                {

                    try
                    {
                        mainForm.serialPort1.Write(send_string);
                    }
                    catch (System.InvalidOperationException)
                    {
                        MessageBox.Show("COM port closed!", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                    catch (System.IO.IOException)
                    {
                        MessageBox.Show("COM port terminated!", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                    mainForm.write_to_send_textbox(send_string);
                }   
            }
        }

        private void SerialSynchroTasK()
        {

        }

        private struct message_constructor
        {
            public char transmitter, reciever;
            public int msg_ID;
            public string replay_function;
            public string replay_cmd;
            public string replay_additional_code;
            public string replay_data;
            public message_constructor(char transmitter_ID, char reciever_ID, int msg_id, string function_string, string command_string, string additional_code_str, string data_string)
            {
                transmitter = transmitter_ID;
                reciever = reciever_ID;
                msg_ID = msg_id;
                replay_function = function_string;
                replay_cmd = command_string;
                replay_additional_code = additional_code_str;
                replay_data = data_string;
            }
        }

        private struct transmitt_buffer
        {
            public int message_ID;            //ser_ID_message v int obliki
            public char transmitt_to_ID;
            public int dirrection;
            public int size;
            public int nack_count;
            public int nack_flag;
            public char[] message;
            public int buffer_counter;
        }
        private struct queue_buffer
        {
            public int command_length;
            public int command_ID;
            public char[] command;
            public int direction;
        }
        public struct transmittOutBuffer
        {
            public string out_str;
            public int out_dir;

            public transmittOutBuffer(string str, int dir)
            {
                out_str = str;
                out_dir = dir;
            }
        }
        queue_buffer[] command_queue_buffer = new queue_buffer[QUEUE_BUFFER_SIZE];//= new queue_buffer[5];
        transmitt_buffer[] Transmitt_handle_Buffer = new transmitt_buffer[TRANSMIT_HANDLE_BUFF_SIZE];
        
        private void command_do_events()
        {
            int queue_cnt;
            int queue_index;
            int queue_index_min = 100;
            do
            {
                event_status = QUEUE_FAIL;
                while (event_status == QUEUE_FAIL)
                {
                    queue_index = QUEUE_INDEX_MAX;
                    for (queue_cnt = 0; queue_cnt < QUEUE_BUFFER_SIZE; queue_cnt++)
                    {
                        if (command_queue_buffer[queue_cnt].command_ID != QUEUE_FREE)
                        {
                            if (command_queue_buffer[queue_cnt].command_ID < queue_index)
                            {
                                queue_index = command_queue_buffer[queue_cnt].command_ID;
                                queue_index_min = queue_cnt;
                            }
                        }
                    }
                    //ce je queue_index ostal na QUEUEUE_INDEX_MAX, potem v polju ni nobenega ukaza vec in lahko prekinemo while zanko
                    //drugace pa izvedemo ukaz na katerega kaze pointer queue_index_min in polje nardimo prosto
                    //--------------------------------------    
                    if (queue_index != QUEUE_INDEX_MAX)
                    {
                        recieved_command_analyze(command_queue_buffer[queue_index_min].command, command_queue_buffer[queue_index_min].direction);
                        command_queue_buffer[queue_index_min].command_ID = QUEUE_FREE;
                    }
                    else
                        event_status = QUEUE_PASS;

                    //-------------------------------------- 
                }
                task1_ex_flag = false;
            } while (task1_ex_flag == true);
        }
        private void recieved_command_analyze(char[] command, int dir)
        {
            //zaenkat se zna bit problem, ce poslje hkrati po dveh vodilih npr PAT in MT skupaj
            int ans;
            ans = ParseMessage(command, dir);
            //ce je v sporocilu acknowladge ali NACK potem ne vracamo ACK sporocila
            if (!(ans > Settings1.Default._ANS_MESSAGE_NACK))
                command_return(ans, dir);
            if (ans == Settings1.Default._ANS_MESSAGE_ACK) command_analyze(dir);  //message analyze 
            if (Settings1.Default.TRANSMIT_COMMAND_CHECK == 1)
            {
                if (ans == Settings1.Default._ANS_MESSAGE_ACK_RECIEVED) Recieve_ack_func();
                else if (ans == Settings1.Default._ANS_MESSAGE_NACK_RECIEVED) Recieve_nack_func();
            }
            
        }


        //PARSE DATA FROM STRING
        //funkcija vrne 0 - vse OK
        //              1 - ni OK NACK
        //              2 - ACK recieved
        //              3 - NACK recieved
        //              4 - ni zame - v tem primeru ne naredimo nic
        private int ParseMessage(char[] command, int transmitter)
        {
            string temp_str = new string(command);
            string message = temp_str.Remove(temp_str.IndexOf((char)0));
            int crc;
            int crcVal;
            int IN_START_TAG_index;
            int token_index;
            int array_nr = 0;
            int[] token_positions = new int[8];
            char delim = ':';
            string[] tokens;
            //message = String.Copy(command);

            //if (transmitter == Settings1.Default._COMMUNICATION_DIR_MT)
            //    IN_START_TAG = IN_START_TAG_MT;
            //else if(transmitter == Settings1.Default._COMMUNICATION_DIR_TFA)
            //    IN_START_TAG = IN_START_TAG_HV;
            tokens = message.Split(delim);
            if (tokens.Length != NUMBER_OF_DELIMITERS+1)
                return 1;
            if (tokens[0].Length != 3 || (!(tokens[1].Length == 2 || tokens[1].Length == 0)))
                return 1;
            m_start_tag = String.Copy(tokens[0]);
            m_msg_ID = String.Copy(tokens[1]);
            m_function = String.Copy(tokens[2]);
            m_command = String.Copy(tokens[3]);
            m_additional_code = String.Copy(tokens[4]);
            m_leftover = String.Copy(tokens[5]);
            m_is_crc = String.Copy(tokens[6]);      //crc sta zadnji in predzadnji podatek, marko je mel to mal zamesan
            m_crc_value = String.Copy(tokens[7]);
            transmitter_ID = m_start_tag[1];
            reciever_ID = m_start_tag[2];

            //ce ni zame skocimo vn
            if (reciever_ID != Settings1.Default._ID_MT)
                return 4;

            

            if (!(m_function.Length > 1)) //pomeni da smo dobili prazen msg
                return 5;

            SetID(int.Parse(m_msg_ID));
                //pomeni da id ni pravilen
            if (!(GetID() >= 0 && GetID() < 100))
                return 1;
            //if (!CheckFunctionExistion(m_function))
            //    return 1;

            //if (!CheckCommandExistion(m_command))
            //    return 1;
            
            if (String.Equals(m_function, Settings1.Default._MESSAGE_ACK))
                return Settings1.Default._ANS_MESSAGE_ACK_RECIEVED;
            else if (String.Equals(m_function, Settings1.Default._MESSAGE_NACK))
                return Settings1.Default._ANS_MESSAGE_NACK_RECIEVED;

            FindAllAdditionalCmdParameters(m_additional_code);

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //nnaslednja koda namenjena testu protokola
            //ce gre za test protokola potem ne naredi nic
            if (String.Equals(m_function, COMMAND_TYPE_NAMES[3]))
            {
                if (String.Equals(m_command, TEST_CODE_NAMES[0]))
                {
                    if (String.Equals(m_leftover, "KRNEKI"))
                    {
                        if (counter1 >= 2)
                        {
                            counter1 = 0;
                        }
                        else
                        {
                            counter1++;
                            return 4;
                        }
                    }
                    else if (String.Equals(m_leftover, "KRNEKI2"))
                    {
                        if (counter2 >= 1)
                        {
                            counter2 = 0;
                        }
                        else
                        {
                            counter2++;
                            return 4;
                        }
                    }
                    else if (String.Equals(m_leftover, "KRNEKI3"))
                    {
                        if (counter3 >= 2)
                        {
                            counter3 = 0;
                        }
                        else
                        {
                            counter3++;
                            return 4;
                        }
                    }
                    else if (String.Equals(m_leftover, "KRNEKI4"))
                    {
                        if (counter4 >= 1)
                        {
                            counter4 = 0;
                        }
                        else
                        {
                            counter4++;
                            return 1;
                        }
                    }
                    else if (String.Equals(m_leftover, "KRNEKI5"))
                    {
                        if (counter5 >= 1)
                        {
                            counter5 = 0;
                        }
                        else
                        {
                            counter5++;
                            return 4;
                        }
                    }
                    else
                    {
                        if (counter6 >= 1)
                        {
                            counter6 = 0;
                        }
                        else
                        {
                            counter6++;
                            return 1;
                        }
                    }
                }
            }
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            crc = int.Parse(m_is_crc);
            crcVal = int.Parse(m_crc_value);
            if (!CheckCRC(crc, crcVal, message))
                return 1;
            //command_analyze(transmitter);
            return 0;
        }

        private void SetID(int id)
        {
            ID = id;
        }

        private int GetID()
        {
            return ID;
        }

        private bool CheckFunctionExistion(string m_func)
        {
            bool FunctionExists = false;
            // if (!String.Compare(m_function, FUNCTION_COMMUNICATON_NAMES[0]))    //tle mors uporabt m_func namest m_function
            //pogleda ce funkcije obstajajo
            if (!String.Equals(m_func, FUNCTION_COMMUNICATON_NAMES[0])) //tole bi loh se izboljsal za ugotavljanje kok jih je v arrayu
                FunctionExists = true;

            if (FunctionExists != true)
                return false;
            else
                return true;
        }

        private bool CheckCommandExistion(string m_cmd)
        {
            bool CommandExists = false;
            for (int i = 0; i < MAX_VALID_COMMANDS; i++)
            {
                if (!String.Equals(m_cmd, COMMAND_TYPE_NAMES[i]))
                    CommandExists = true;
            }
            if (CommandExists != true)
                return false;
            else
                return true;
        }

        private bool CheckCRC(int isCRC, int CRCvalue, string message)
        {
            if (isCRC == 1)
            {
                if (CRCvalue <= 255) //CRCvalue>=0 && CRCvalue<=255
                {
                    int CRCresult = CalculateCRC(message);     // racunamo crc do petega delimiterja
                    if (CRCresult == CRCvalue)
                        return true;
                    else
                        return false;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return true;
            }
        }
        private int CalculateCRC(string message)
        {
            //	int vsota = 0;	
            //	for (int i = 0 ; i < strlen(message); i++)
            //		vsota += message[i];
            //		
            //	return (vsota%256);
            int podpi = 0;
            int vsota = 0;
            for (int a = 0; a < message.Length; a++)
            {
                if (message[a] == ':')
                {
                    delimiterArray[podpi] = a;
                    podpi++;
                }
            }
            delimiterArray[podpi] = message.Length;
            for (int a = 0; ((a < delimiterArray[6] + 1) && (a < message.Length)); a++)
                vsota = vsota + (int)message[a];//message[a];

            return (vsota % 256);
        }

        /*******************************************************************************/
        /**								ANALIZE COMMAND								  **/
        /*******************************************************************************/
        private void command_analyze(int dir)
        {


                /*******************************************************************************/
                /**								MEASURING METOD 								**/
                /*******************************************************************************/
                if (String.Equals(m_function, COMMAND_TYPE_NAMES[0]) == true)///POWER
                {


                }
                /*******************************************************************************/
                /**									RELAYS									**/
                /*******************************************************************************/
                else if (String.Equals(m_function, COMMAND_TYPE_NAMES[1]) == true)
                {
                }
                /*******************************************************************************/
                /**									COMMUNCATION   							  **/
                /*******************************************************************************/
                else if (String.Equals(m_function, COMMAND_TYPE_NAMES[4]) == true)//communication
                {
                    if (String.Equals(m_command, CONNECTION_CODE_NAMES[1]) == true) //connected
                    {
                        mainForm.AdapterCOM_status.BackColor = Color.LightGreen;
                    }
                    else if (String.Equals(m_command, CONNECTION_CODE_NAMES[2]) == true) //connected
                    {
                        message_to_send = 1;
                        task3 = new Thread(send_message);
                        task3.Start();
                    }
                }
                /*******************************************************************************/
                /**									CORD          							  **/
                /*******************************************************************************/
                else if (String.Equals(m_function, COMMAND_TYPE_NAMES[5]))//cord
                {
                    if (String.Equals(m_command, CORD_CODE_NAMES[3])) //get resistance
                    {
                        cord_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, CORD_CODE_NAMES[5])) //stopped
                    {
                        cord_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, CORD_CODE_NAMES[6])) //initiated
                    {
                        cord_return_event("", m_command);
                    }
                    else if (String.Equals(m_command, CORD_CODE_NAMES[7])) //RISO_ALL-PE
                    {
                        if ((String.Equals(additionalCode[0, 0], "PASS"))||(String.Equals(additionalCode[0, 0], "FAIL")))
                            cord_return_event(additionalCode[0, 0], m_command);
                        else
                            cord_return_event("", m_command);
                    }
                    else if (String.Equals(m_command, CORD_CODE_NAMES[8])) //RISO_ONE-PE
                    {
                        if ((String.Equals(additionalCode[0, 0], "PASS")) || (String.Equals(additionalCode[0, 0], "FAIL")))
                            cord_return_event(additionalCode[0, 0], m_command);
                        else
                            cord_return_event("", m_command);
                    }
                    else if (String.Equals(m_command, CORD_CODE_NAMES[9])) //RISO_PH-PH
                    {
                        if ((String.Equals(additionalCode[0, 0], "PASS")) || (String.Equals(additionalCode[0, 0], "FAIL")))
                            cord_return_event(additionalCode[0, 0], m_command);
                        else
                            cord_return_event("", m_command); cord_return_event("", m_command);
                    }
                    else if (String.Equals(m_command, CORD_CODE_NAMES[17])) //get RISO resistance
                    {
                        cord_return_event(m_leftover, m_command);
                    }
                    else
                    {
                        cord_return_event("", m_command);
                    }
                }
                /*******************************************************************************/
                /**								MACHINES         							  **/
                /*******************************************************************************/
                else if (String.Equals(m_function, COMMAND_TYPE_NAMES[7])) //mach
                {
                    if (String.Equals(m_command, MACH_COMMAND_NAMES[6])) //STOP
                    {
                        mach_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, MACH_COMMAND_NAMES[9])) //INITIATED
                    {
                        mach_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, MACH_COMMAND_NAMES[3])) //URES STARTED
                    {
                        mach_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, MACH_COMMAND_NAMES[4])) //URES STOPPED
                    {
                        mach_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, MACH_COMMAND_NAMES[8])) //URES OPENED
                    {
                        mach_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, MACH_COMMAND_NAMES[13])) //RPE STARTED
                    {
                        mach_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, MACH_COMMAND_NAMES[14])) //RPE STOPPED
                    {
                        mach_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, MACH_COMMAND_NAMES[17])) //ALL-PE RESULT
                    {
                        mach_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, MACH_COMMAND_NAMES[18])) //ONE-PE RESULT
                    {
                        mach_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, MACH_COMMAND_NAMES[23])) //get RISO resistance
                    {
                        mach_return_event(m_leftover, m_command);
                    }
                    else
                    {
                       mach_return_event("", m_command);
                    }
                }
                /*******************************************************************************/
                /**								WELDING         							  **/
                /*******************************************************************************/
                else if (String.Equals(m_function, COMMAND_TYPE_NAMES[8])) //weld
                {
                    if (String.Equals(m_command, WELD_COMMAND_NAMES[6])) //STOP
                    {
                        weld_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, WELD_COMMAND_NAMES[9])) //INITIATED
                    {
                        weld_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, WELD_COMMAND_NAMES[3])) //URES STARTED
                    {
                        weld_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, WELD_COMMAND_NAMES[4])) //URES STOPPED
                    {
                        weld_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, WELD_COMMAND_NAMES[8])) //URES OPENED
                    {
                        weld_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, WELD_COMMAND_NAMES[13])) //RPE STARTED
                    {
                        weld_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, WELD_COMMAND_NAMES[14])) //RPE STOPPED
                    {
                        weld_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, WELD_COMMAND_NAMES[17])) //ALL-PE RESULT
                    {
                        weld_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, WELD_COMMAND_NAMES[18])) //ONE-PE RESULT
                    {
                        weld_return_event(m_leftover, m_command);
                    }
                    else if (String.Equals(m_command, WELD_COMMAND_NAMES[23])) //get RISO resistance
                    {
                        weld_return_event(m_leftover, m_command);
                    }
                    else
                    {
                        weld_return_event("", m_command);
                    }
                }
        }
        public delegate void cord_return_result(object sender, string meas_con, string cmd);
        public event cord_return_result cordReturnEventHandler = delegate { };
        public void cord_return_event(string return_string, string cmd)
        {
            this.cordReturnEventHandler(this, return_string,cmd);
        }

        public delegate void mach_return_result(object sender, string meas_con, string cmd);
        public event mach_return_result machReturnEventHandler = delegate { };
        public void mach_return_event(string return_string, string cmd)
        {
            this.machReturnEventHandler(this, return_string, cmd);
        }

        public delegate void weld_return_result(object sender, string meas_con, string cmd);
        public event weld_return_result weldReturnEventHandler = delegate { };
        public void weld_return_event(string return_string, string cmd)
        {
            this.weldReturnEventHandler(this, return_string, cmd);
        }

        private void send_message()
        {
            if (message_to_send == 1)
            {
                Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, COMMAND_TYPE_NAMES[4], CONNECTION_CODE_NAMES[2], CONNECTION_ADD_NAMES[1],"");
                message_to_send = 0;
            }
        }
        //return answer (acknowledge or not acknowledge)
        private void command_return(int ans, int dir)
        {
            int checksum;
            string checksum_str;
            string temp_str;
            string ser_ans_buffer;
            string temp_ID;
            if (ID < 10)
                temp_ID = String.Format("0{0,1}", ID);
            else temp_ID = ID.ToString();
            switch (ans)
            {
                case _ANS_MESSAGE_NACK: temp_str = String.Format("{0}{1}{2}:{3}:{4}::::1:", Settings1.Default._SER_START_SIGN, Settings1.Default._ID_MT, transmitter_ID, temp_ID, Settings1.Default._MESSAGE_NACK); break;
                case _ANS_MESSAGE_ACK: temp_str = String.Format("{0}{1}{2}:{3}:{4}::::1:", Settings1.Default._SER_START_SIGN, Settings1.Default._ID_MT, transmitter_ID, temp_ID, Settings1.Default._MESSAGE_ACK); break;
                default: temp_str = ""; break;
            }
            checksum = CalculateCRC(temp_str);
            checksum_str = checksum.ToString();
            ser_ans_buffer = String.Format("{0}{1}:{2}{3}", temp_str, checksum_str, (char)13, (char)10);
            serial_send(dir, ser_ans_buffer);
            //tuki mormo poslat

        }
        
        private void FindAllAdditionalCmdParameters(string add_param)
        {
            int i;
            string[] add_parameters;
            string[] func_value;

            add_parameters = add_param.Split(',');

            for (i = 0; i < add_parameters.Length; i++)
            {
                func_value = add_parameters[i].Split('|');
                if (func_value.Length == 1)
                {
                    additionalCode[0, i] = func_value[0];
                    additionalCode[1, i] = "";
                }
                else
                {
                    additionalCode[0, i] = func_value[0];   //v eno vrstico se shran funkcija  
                    additionalCode[1, i] = func_value[1];   //v drugi vrstici je value
                }
            }
        }

        
        private string ConstructReplyMsg(message_constructor trans_message)
        {
            string new_ID;
            if (trans_message.msg_ID < 10)
                new_ID = String.Format("0{0,1}", trans_message.msg_ID);
            else
                new_ID = String.Format("{0,2}", trans_message.msg_ID);
            string temp_str = String.Format(">{0}{1}:{2,2}:{3}:{4}:{5}:{6}:1:", Settings1.Default._ID_MT, trans_message.reciever, new_ID, trans_message.replay_function, trans_message.replay_cmd, trans_message.replay_additional_code, trans_message.replay_data);
            string temp_str2 = (String.Format("{0}{1,3}:{2}{3}", temp_str, CalculateCRC(temp_str), UART_COMMAND_CHARACTER_13, UART_COMMAND_CHARACTER_10));
            return temp_str2;
        }
        public bool CheckIfACK(int msg_id)
        {
            int i;
            for (i = 0; i < TRANSMIT_HANDLE_BUFF_SIZE; i++)
                if (Transmitt_handle_Buffer[i].message_ID == msg_id)
                    return false;
            return true;
        }
        private int Recieve_ack_func()
        {
            int index = 500;
            string new_str;
            if ((ID < 100) && (ID > 0))
            {
                for (int i = 0; i < TRANSMIT_HANDLE_BUFF_SIZE; i++)
                {
                    if (Transmitt_handle_Buffer[i].message_ID == ID)   //poiscemo kje se nahaja sporocilo s tem idjem
                    {
                        index = i;
                        new_str = String.Format("Message {0} ACKNOWLADGE!", Transmitt_handle_Buffer[i].message_ID.ToString());
                        Debug.WriteLine(new_str);
                        i = TRANSMIT_HANDLE_BUFF_SIZE; //skocimo ven iz for zanke
                    }
                }
                if (index != 500)
                {
                    Transmitt_handle_Buffer[index].message_ID = Settings1.Default.TRANSMIT_SLOT_FREE;
                    Transmitt_handle_Buffer[index].nack_flag = 0;
                    Transmitt_handle_Buffer[index].nack_count = 0;
                    Transmitt_handle_Buffer[index].size = 0;
                }
                else return 2;	//pomeni da sporocila s takim id-jem ni v cakalni vrsti
            }
            else return 3;
            return 0;
        }
        private int Recieve_nack_func()
        {
            int index = 500;
            if ((ID < 100) && (ID > 0))
            {
                for (int i = 0; i < TRANSMIT_HANDLE_BUFF_SIZE; i++)
                {
                    if (Transmitt_handle_Buffer[i].message_ID == ID)   //poiscemo kje se nahaja sporocilo s tem idjem
                    {
                        index = i;
                        i = TRANSMIT_HANDLE_BUFF_SIZE; //skocimo ven iz for zanke
                    }
                }
                if (index != 500)
                    Transmitt_handle_Buffer[index].nack_flag = 1;
                else return 2;	//pomeni da sporocila s takim id-jem ni v cakalni vrsti
            }
            else return 3;
            return 0;
        }
        private int Transmit_func(int message_size, int dir, char[] ser_ans_buff, int current_ID,char receiver)
        {
            Transmitt_handle_Buffer[transmitt_handle_write_count].message_ID = current_ID;
            Transmitt_handle_Buffer[transmitt_handle_write_count].transmitt_to_ID = receiver;
            Transmitt_handle_Buffer[transmitt_handle_write_count].dirrection = dir;
            Transmitt_handle_Buffer[transmitt_handle_write_count].message = ser_ans_buff;
            Transmitt_handle_Buffer[transmitt_handle_write_count].buffer_counter = 0;
            Transmitt_handle_Buffer[transmitt_handle_write_count].size = message_size;
            Array.Copy(ser_ans_buff, Transmitt_handle_Buffer[transmitt_handle_write_count].message, message_size);
            //------------------------------preveri ce je buffer poln-------------------------------
            if ((transmitt_handle_write_count == (TRANSMIT_HANDLE_BUFF_SIZE - 1)) && (transmitt_handle_read_count == 0)) return 2;
            else if (transmitt_handle_write_count == (transmitt_handle_read_count - 1)) return 2;
            //--------------------------------------------------------------------------------------
            //ce je poln ne povecamo stevca ampak smo se zmeri na zadnjem mestu
            else if (transmitt_handle_write_count >= (TRANSMIT_HANDLE_BUFF_SIZE - 1)) transmitt_handle_write_count = 0;
            else transmitt_handle_write_count++;
            //omogoci, da ce posiljamo vec podatkov naenkrat timer zalavfa sele po zadnjem, kar da tudi zadnjemu 10ms casa da dobi ACK
            //if (task2.ThreadState == ThreadState.Unstarted) //ce task se ni zagnan ga zazenemo
            //    task2.Start();
            if (task2_ex_flag == false)
            {
                task2_ex_flag = true;
                task2 = new Thread(transmit_command_handle);
                task2.Start();
                //Thread.Sleep(10);                               //vsako poslano sporocilo ponovno zazene sleep, kar da tudi zadnjemu 10ms casa da dobi ACK
            }
            return 0;

        }
        private void transmit_command_handle()
        {
            char[] temp_array = new char[3];
            int j = 0;
            int i = 0;
            int command_not_ok_flag = 0;
            string temp_str;
            //preveri ce je buffer prazen
            do
            {
                Thread.Sleep(100);
                if (transmitt_handle_read_count == transmitt_handle_write_count)
                {
                    task2_ex_flag = false;
                }
                else
                {
                    //ce je prostor na katerega kaze read_count dobil ACKNOWLADGE potem se izvede naslednje,
                    //ki sprosti buffer iz HEAP spomina, ki rola toliko casa da izprazne ciklicen buffer. 
                    //Ce naleti na poln slot, kar pomeni da to sporocilo se ni dobilo ACK zato ga poslje ponovno
                    //nato pa se celoten task izvede ponovno, kar se ponavlja dokler ni buffer za posiljanje povsem prazen
                    if (Transmitt_handle_Buffer[transmitt_handle_read_count].message_ID == Settings1.Default.TRANSMIT_SLOT_FREE)
                    {
                        do
                        {
                            Transmitt_handle_Buffer[transmitt_handle_read_count].message_ID = Settings1.Default.TRANSMIT_SLOT_FREE;
                            Transmitt_handle_Buffer[transmitt_handle_read_count].nack_flag = 0;
                            Transmitt_handle_Buffer[transmitt_handle_read_count].nack_count = 0;
                            Transmitt_handle_Buffer[transmitt_handle_read_count].size = 0;
                            Transmitt_handle_Buffer[transmitt_handle_read_count].buffer_counter = 0;

                            //postavimo se na naslednji vnos bufferja in preverimo ce je tudi ta prazen ce ni zapustimo zanko
                            if ((transmitt_handle_read_count + 1) == TRANSMIT_HANDLE_BUFF_SIZE) transmitt_handle_read_count = 0;//gre na zacetek ker je ciklicen buffer
                            else transmitt_handle_read_count++;
                            if (transmitt_handle_read_count == transmitt_handle_write_count) break;                       //buffer spraznjen
                        } while (Transmitt_handle_Buffer[transmitt_handle_read_count].message_ID == Settings1.Default.TRANSMIT_SLOT_FREE);
                        //ce pridemo iz zgornje funkcije preden se buffer sprazni pomeni da smo naleteli na ID, ki se ni dobil ACK zato ga ponovno posljemo
                        if (transmitt_handle_read_count != transmitt_handle_write_count)
                            if(Transmitt_handle_Buffer[transmitt_handle_read_count].buffer_counter>Settings1.Default._TIME_FOR_ACK)
                                command_not_ok_flag = 1;
                    }
                    else
                        if (Transmitt_handle_Buffer[transmitt_handle_read_count].buffer_counter > Settings1.Default._TIME_FOR_ACK)
                            command_not_ok_flag = 1;
                    //ce buffer se ni prazen pocaka 10 ms in ponovno izvede nit
                    //if (transmitt_handle_read_count != transmitt_handle_write_count)
                    //    Thread.Sleep(10);
                    if (transmitt_handle_read_count == transmitt_handle_write_count) task2_ex_flag = false;
                }
                //ko pride komanda tle not pomeni, da je ze dovolj dolgo v cakalni vrsti in se ni dobila ACKNOWLADga
                if (command_not_ok_flag == 1)
                {
                    j = 0;
                    //	do	//tole bi prslo v postev ce zelimo pogledat se po cakalni vrsti naprej in poslati vse ki se niso dobili ACK v enem tasku
                    //	{
                    //		if(Transmit_handle_buff[read_count+j].message_ID!=TRANSMIT_SLOT_FREE)
                    string temp_str2 = new string(Transmitt_handle_Buffer[transmitt_handle_read_count + j].message);
                    serial_send(Transmitt_handle_Buffer[transmitt_handle_read_count + j].dirrection, temp_str2);
                    if (Transmitt_handle_Buffer[transmitt_handle_read_count + j].nack_count >= NUM_NACK_EVENTS)
                    {
                        //pretvorba int v str veliko hitrejsa kot sprintf
                        if ((Transmitt_handle_Buffer[transmitt_handle_read_count + j].message_ID < 100) && (Transmitt_handle_Buffer[transmitt_handle_read_count + j].message_ID > 0))
                        {
                            temp_str = Transmitt_handle_Buffer[transmitt_handle_read_count + j].message_ID.ToString();
                        }
                        //sprintf(temp_array,"%u.2",Transmit_handle_buff[read_count].message_ID);
                        //1X poslje warning in pobrise sporocilo iz vrste za posiljanje
                        //--------ce hoces posiljanje mesiđa po protokolu potem odkomentiraj naslednjo vrstico in zakomentiraj naslednje 3---------
                        //Send_protocol_message(Transmitt_handle_Buffer[transmitt_handle_read_count + j].dirrection, Settings1.Default._ID_MT, Transmitt_handle_Buffer[transmitt_handle_read_count + j].transmitt_to_ID, FUNCTION_COMMUNICATON_NAMES[0], COMMAND_TYPE_NAMES[2], WARNING_CODE_NAMES[0], Transmitt_handle_Buffer[transmitt_handle_read_count + j].message_ID.ToString());
                        message_constructor warning_constructor = new message_constructor(Settings1.Default._ID_MT, Transmitt_handle_Buffer[transmitt_handle_read_count + j].transmitt_to_ID, 99, COMMAND_TYPE_NAMES[2], WARNING_CODE_NAMES[0], Transmitt_handle_Buffer[transmitt_handle_read_count + j].message_ID.ToString(),"");
                        string warning_message = ConstructReplyMsg(warning_constructor);
                        serial_send(Transmitt_handle_Buffer[transmitt_handle_read_count + j].dirrection, warning_message);
                        //ce posiljanje v tretje ne uspe zbrisemo podatke in gremo naprej
                        Transmitt_handle_Buffer[transmitt_handle_read_count + j].message_ID = Settings1.Default.TRANSMIT_SLOT_FREE;
                        Transmitt_handle_Buffer[transmitt_handle_read_count + j].nack_flag = 0;
                        Transmitt_handle_Buffer[transmitt_handle_read_count + j].nack_count = 0;
                        Transmitt_handle_Buffer[transmitt_handle_read_count + j].size = 0;
                        if ((transmitt_handle_read_count + 1) == TRANSMIT_HANDLE_BUFF_SIZE) transmitt_handle_read_count = 0;//gre na zacetek ker je ciklicen buffer prisel do konca
                        else transmitt_handle_read_count++;
                    }
                    else
                    {
                        Transmitt_handle_Buffer[transmitt_handle_read_count].nack_count++;
                        //		i++;
                        //	}while((read_count+j)!=write_count);

                        //pogleda se naslednje komande v bufferju ce so slucajno dobili NACK in ponovi posiljanje se za te
                        if ((transmitt_handle_read_count + 1) == TRANSMIT_HANDLE_BUFF_SIZE) i = 0;//gre na zacetek ker je ciklicen buffer
                        else i = transmitt_handle_read_count + 1;
                        for (; i != transmitt_handle_write_count;)
                        {
                            //pogleda ce ima postavljeno NACK zastavico, ce jo ima se sporocilo ponovno poslje in zastavica pobrise
                            if (Transmitt_handle_Buffer[i].nack_flag == 1)
                            {
                                serial_send(Transmitt_handle_Buffer[i].dirrection, new string(Transmitt_handle_Buffer[i].message));
                                Transmitt_handle_Buffer[i].nack_flag = 0;
                                Transmitt_handle_Buffer[i].nack_count++;
                            }
                            if ((i + 1) == TRANSMIT_HANDLE_BUFF_SIZE) i = 0;
                            else i++;
                        }
                    }
                    command_not_ok_flag = 0;
                }
                //povecam counter tistim ukazom, ki niso prazni
                for (int k=0; k< TRANSMIT_HANDLE_BUFF_SIZE;k++)
                    if(Transmitt_handle_Buffer[k].message_ID != Settings1.Default.TRANSMIT_SLOT_FREE)
                        Transmitt_handle_Buffer[k].buffer_counter++;
            } while (task2_ex_flag == true);
        }
    }
}
