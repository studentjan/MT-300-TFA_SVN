using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.IO.Ports;
using System.Windows.Forms;
using System.Threading;

namespace MT_300_TFA_application
{
    public partial class Form1 : Form
    {
        public String[] AvailibleCOM;
        public string RxLine;
        public string RxLine2;
        public bool AdapterButtonStatus;
        public serial_com Serial_class;// = new serial_com();
        public Relay_form Relay_object;// = new Relay_form();
        public Cord_form cord_object;
        public Machines_form Machines_object;
        private Queue<char[]> receiveQueue = new Queue<char[]>();
        private int prev_i = 0;
        //public serial_com Serial_class = new serial_com();
        //public Relay_form RelayForm;
        //Thread nit1;
        public bool double_buffer = false;
        public string[] RxString= new String[20];
        public char[] RxCharArray = new char[300];
        public int recieved_num;
        public int j = 0;
        byte[] buffer = new byte[500];
        byte[] buffer2 = new byte[250];
        byte[] buffer3 = new byte[250];

        public Form1()
        {
            //cord_object = new Cord_form(Serial_class);
            Serial_class = new serial_com(this);
            InitializeComponent();
            getAvailiblePorts();
            this.terminal_textbox.Font = new Font(terminal_textbox.Font.FontFamily, 8);
            this.terminal_textbox.Clear();
            Serial_class.SerialRx_Init();
            //Relay_object = new Relay_form(this, Serial_class);
            CodeRed();
        }
        private void Form1_Load(object sender, EventArgs e)  //TOLE SE TI ZLOVDA OB STARTU APPA. CE BOS MEL SAM EN COM BOS MEL SAM serialPort1.data.... ostalo zbris
        {
            //this.FormBorderStyle = FormBorderStyle.FixedSingle;  //to nardi window non resizable, torej če hočš vidt debug text box tole zakomenteri pa raztegn okn
            serialPort1.DataReceived += serialPort1_DataReceived;
        }

        private void port_connect_Click(object sender, EventArgs e)
        {
            if (AdapterButtonStatus != true)
            {
                try
                {
                    serialPort1.Close();
                    serialPort1.PortName = avalible_ports_combobox.Text;
                }
                catch (System.ArgumentException)
                {
                    MessageBox.Show("Please select COM port", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    AdapterButtonStatus = true;
                    return;
                }
                try
                {
                    serialPort1.BaudRate = 115200;
                    serialPort1.Parity = Parity.None;
                    serialPort1.StopBits = StopBits.One;
                    serialPort1.Handshake = Handshake.None;
                    serialPort1.ReceivedBytesThreshold = 18;
                    serialPort1.Open();
                    AdapterButtonStatus = true;
                    port_connect.Text = "DISCONNECT";
                    Serial_class.synchroTimer.Start();
                        //na zacetku enkrat zazenemo timer zato, da posljemo zacetno komando.
                    Serial_class.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                        Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.COMMAND_TYPE_NAMES[4],
                        serial_com.CONNECTION_CODE_NAMES[0], serial_com.CONNECTION_ADD_NAMES[0], "");
                }
                catch (System.UnauthorizedAccessException)
                {
                    MessageBox.Show("COM port in use", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                catch (System.IO.IOException)
                {
                    MessageBox.Show("Device is not responding!", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }

            }
            else
            {
                AdapterButtonStatus = false;
                serialPort1.Close();
                port_connect.Text = "CONNECT";
                AdapterCOM_status.BackColor = Color.Red;
            }
        }

        void getAvailiblePorts()
        {
            AvailibleCOM = SerialPort.GetPortNames();
            avalible_ports_combobox.Items.Clear();
            avalible_ports_combobox.Items.AddRange(AvailibleCOM);
        }
        private void CodeRed()
        {
            AdapterCOM_status.BackColor = Color.Red;
        }
        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)  //to je tvoja funkcija za com receive handler... nardis eventhandler zato da spustis funkcijo, tam notr mas pa compare (podobn k set_event v cju)
        {
            //serialPort1.Read(buffer, 0, serialPort1.BytesToRead);
            //byte[] arr = {13, 10};
            //int strlen = Array.IndexOf(buffer, arr);
            //Array.Copy(buffer, buffer2,strlen);
            //buffer2[((Array.IndexOf(buffer, 10)) + 1)] = 0;
            //if ((serialPort1.BytesToRead) > ((Array.IndexOf(buffer, 10)) + 1))
            //{
            //    if (!((-1) == (Array.IndexOf(buffer, 10, (Array.IndexOf(buffer, 10) + 1)))))
            //    {
            //        Array.ConstrainedCopy(buffer, ((Array.IndexOf(buffer, 10)) + 1), buffer3, ((Array.IndexOf(buffer, 10, (Array.IndexOf(buffer, 10) + 1)) + 1)), ((Array.IndexOf(buffer, 10, (Array.IndexOf(buffer, 10)))) - (Array.IndexOf(buffer, 10))));
            //        buffer3[(Array.IndexOf(buffer, 10, (Array.IndexOf(buffer, 10) + 1)) + 1)] = 0;
            //        RxLine2 = System.Text.Encoding.ASCII.GetString(buffer3);
            //        double_buffer = true;
            //    }
            //    else
            //    {
            //        RxLine2 = "\0";
            //        double_buffer = false;
            //    }
            //}
            //else
            //    double_buffer = false;


            //RxLine = System.Text.Encoding.ASCII.GetString(buffer2);
            int byte_num = serialPort1.BytesToRead;
            int byte_used=0;
            char temp_char;
            char temp_char_prev= '0';
            //RxLine = serialPort1.ReadLine();
            double_buffer = false;
            int i;
            int i2 = 0;
            int temp_prev_i=0;
            int char_offset=0;
            //for (i=0; byte_num> byte_used; i++)
            //{
            //    RxString[i]= (serialPort1.ReadLine()).Replace("\r", "\r\n");    //read line odstrani \n zato ga spet dodamo
            //    byte_used += RxString[i].Length;
            //    //krneki = serialPort1.BytesToRead;
            //    //RxLine2 = serialPort1.ReadLine();
            //    //double_buffer = true;
            //}
            for (i = 0; i < byte_num; i++)
            {
                temp_char = Convert.ToChar(serialPort1.ReadChar());//pretvorimo v ascii
                //if (temp_char == '>') char_offset = i-i2;      //ko naleti na prvi start znak zacne pisati na novo
                if (temp_char == '\0')
                    char_offset++;
                else
                {
                    RxCharArray[((prev_i + i) - i2) - char_offset] = temp_char;
                    temp_prev_i++;
                }
                if (temp_char_prev == '\r' && temp_char == '\n')
                {
                    RxCharArray[i + 1] = '\0';
                    receiveQueue.Enqueue(RxCharArray);//damo prejsno komando v cakalno vrsto
                    i2 = i+1;               //ko zacnemo vpisovati v nov array moramo zaceti na zacetku, zato shranimo offset
                    temp_prev_i = 0;             //ce zapustimo zanko in se nismo dobili \n potem zelimo naslednjic nadaljevati od tam od kjer smo nazadnje koncali
                    char_offset = 0;
                }
                temp_char_prev = temp_char;
            }
            prev_i = temp_prev_i;
               // recieved_num = i;
            this.Invoke(new EventHandler(TaskManager));
        }

        private void TaskManager(object sender, EventArgs e)
        {
            //Dim Newline As String
            //Newline = System.Environment.NewLine;
            //terminal_textbox.AppendText(RxLine);
            //terminal_textbox.AppendText(Environment.NewLine);
            //terminal_textbox.Text = terminal_textbox.Text  & Newline;
            //ReadLine() odreze \n zato ga nadomestimo

            //string new_RxLine;
            //string new_RyLine2;
            //if (double_buffer == false)
            //{
            //    new_RxLine = RxLine.Replace("\r", "\r\n");
            //    Serial_class.add_command_to_queue(new_RxLine, new_RxLine.Length, serial_com.COMM_DIR_PORT1);
            //    terminal_textbox.AppendText(new_RxLine);
            //}
            //else
            //{
            //    new_RxLine = RxLine.Replace("\r", "\r\n");
            //    new_RyLine2 = RxLine2.Replace("\r", "\r\n");
            //    Serial_class.add_command_to_queue(new_RxLine, new_RxLine.Length, serial_com.COMM_DIR_PORT1);
            //    Serial_class.add_command_to_queue(new_RyLine2, new_RyLine2.Length, serial_com.COMM_DIR_PORT1);
            //    terminal_textbox.AppendText(new_RxLine);
            //    terminal_textbox.AppendText(new_RyLine2);
            //}

            //int i;
            while (receiveQueue.Count != 0)
            {
                string temp_str3;
                
                char[] temp_str = new char[300];
                temp_str= receiveQueue.Dequeue();

                string temp_str2 = new string(temp_str);//temp_str.Substring(0,(temp_str.Length));
                temp_str3 = temp_str2.TrimStart('\0');
                string[] temp_str4 = temp_str3.Split('\0');
                Serial_class.add_command_to_queue(temp_str4[0], temp_str4[0].Length, serial_com.COMM_DIR_PORT1);
                terminal_textbox.AppendText(temp_str4[0]);
            }
            //for(i=0; i<recieved_num;i++)
            //{
            //    Serial_class.add_command_to_queue(RxString[i], RxString[i].Length, serial_com.COMM_DIR_PORT1);
            //    terminal_textbox.AppendText(RxString[i]);
            //}
            //if (double_buffer == false)
            //{
            //    terminal_textbox.AppendText(RxLine);
            //    terminal_textbox.AppendText(Environment.NewLine);
            //    Serial_class.add_command_to_queue(RxLine, RxLine.Length, serial_com.COMM_DIR_PORT1);    //novo komando da v cakalno vrsto za analiziranje
            //}
            //else
            //{
            //    terminal_textbox.AppendText(RxLine);
            //    terminal_textbox.AppendText(Environment.NewLine);
            //    terminal_textbox.AppendText(RxLine2);
            //    terminal_textbox.AppendText(Environment.NewLine);
            //    Serial_class.add_command_to_queue(RxLine, RxLine.Length, serial_com.COMM_DIR_PORT1);
            //    Serial_class.add_command_to_queue(RxLine2, RxLine2.Length, serial_com.COMM_DIR_PORT1);
            //}

            //terminal_textbox.

        }
        delegate void write_to_send_textbox_Callback(string message);

        public void write_to_send_textbox(string message)
        {
            if(this.terminal_textbox_send.InvokeRequired)
            {
                write_to_send_textbox_Callback d = new write_to_send_textbox_Callback(write_to_send_textbox);
                this.Invoke(d, new object[] { message });
            }
            else
            {
                this.terminal_textbox_send.AppendText(message);
                //this.terminal_textbox_send.AppendText(Environment.NewLine);
            }
        }

        private void test_com_protocol_button_Click(object sender, EventArgs e)
        {
            Serial_class.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.COMMAND_TYPE_NAMES[3], serial_com.TEST_CODE_NAMES[0], "","");
        }

        private void cord_button_Click(object sender, EventArgs e)
        {
            //Serial_class.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[2], "");
            Cord_form cordForm = new Cord_form(Serial_class,this);
            cordForm.Show();
            machines_button.Enabled = false;
            rescan_button.Enabled = false;
            test_com_protocol_button.Enabled = false;
            test_relays_button.Enabled = false;
            port_connect.Enabled = false;
            cord_button.Enabled = false;
        }

        private void test_relays_button_Click(object sender, EventArgs e)
        {
            Relay_form RelayForm = new Relay_form(this, Serial_class);
            RelayForm.Show();
            machines_button.Enabled = false;
            rescan_button.Enabled = false;
            test_com_protocol_button.Enabled = false;
            test_relays_button.Enabled = false;
            port_connect.Enabled = false;
            cord_button.Enabled = false;
        }

        private void clear_send_button_Click(object sender, EventArgs e)
        {
            terminal_textbox_send.Clear();
        }

        private void clear_receive_button_Click(object sender, EventArgs e)
        {
            terminal_textbox.Clear();
        }

        private void rescan_button_Click(object sender, EventArgs e)
        {
            getAvailiblePorts();
        }

        private void machines_button_Click(object sender, EventArgs e)
        {
            Machines_form MachinesObject = new Machines_form(Serial_class,this);
            MachinesObject.Show();
            machines_button.Enabled = false;
            rescan_button.Enabled = false;
            test_com_protocol_button.Enabled = false;
            test_relays_button.Enabled = false;
            port_connect.Enabled = false;
            cord_button.Enabled = false;
        }
        //private void Sendtextboxmaneger()
        //{

        //}
    }
}
