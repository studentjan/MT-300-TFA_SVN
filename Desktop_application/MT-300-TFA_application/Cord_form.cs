using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Threading;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;
using System.Windows.Forms;

namespace MT_300_TFA_application
{
    public partial class Cord_form : Form
    {
        public bool CordMeasInProg = false;
        private serial_com Serial_object;
        private Thread task1;
        string current_meas;
        private bool task1_ex;
        private string _selectedStateTxt;
        private string _selectedTypeTxt;
        private string _failReturnTXT;
        private string _passReturnTXT;
        private string delimiter = "|";
        private string prev_msg = "";

        public Cord_form(serial_com S_object)
        {
            InitializeComponent();
            Serial_object = S_object;
            Serial_object.cordReturnEventHandler += new serial_com.cord_return_result(OnCordReturnResult);
            task1 = new Thread(cord_basic_task);
            Cable_state_comboBox.SelectedIndex = 0;
            Cable_type_comboBox.SelectedIndex = 0;
            _selectedStateTxt = Cable_state_comboBox.Text;
            _selectedTypeTxt = Cable_type_comboBox.Text;
            resistance_return_value.AppendText("0.2");
            fail_return_textbox.AppendText("5.0");
            _failReturnTXT = fail_return_textbox.Text;
            _passReturnTXT = resistance_return_value.Text;
        }

        private void polarity_test_start_button_Click(object sender, EventArgs e)
        {
            if (CordMeasInProg == false)
            {
                CordMeasInProg = true;
                meas_in_progress_ind.BackColor = Color.Red;
                polarity_test_start_button.Text = "STOP";
                //zazenemo meritev
                if(String.Equals(_selectedTypeTxt, "1 PHASE"))
                    Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[0], serial_com.CORD_LEFTOVER_NAMES[25]);
                else if (String.Equals(_selectedTypeTxt, "3 PHASE"))
                    Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[0], serial_com.CORD_LEFTOVER_NAMES[26]);
            }
            else
            {
                //ustavimo meritev
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[1], "");
                //set_stop_cord();
            }
        }
        private void set_stop_cord()
        {
            meas_in_progress_ind.BackColor = Color.Empty;
            write_to_test_start_button("START");
            CordMeasInProg = false;
        }
        
        public void OnCordReturnResult(object sender, string returned_string, string command)
        {
            if(String.Equals(command, serial_com.CORD_CODE_NAMES[3]))        //get resistance
            {
                cord_return_result(returned_string);
            }
            else if (String.Equals(command, serial_com.CORD_CODE_NAMES[5]))  //stopped
            {
                set_stop_cord();
            }
        }
        public void cord_return_result(string meas_con)
        {
            if (!String.Equals(prev_msg, meas_con))
            {
                //if (task1_ex == false)
                //{
                prev_msg = meas_con;
                //task1_ex = true;
                task1 = new Thread(cord_basic_task);
                task1.Start();
                current_meas = meas_con;

                //}
            }
        }
        private void cord_basic_task()
        {
            string failTempString = serial_com.CORD_CODE_NAMES[4] + delimiter + _failReturnTXT;
            string passTempString = serial_com.CORD_CODE_NAMES[4] + delimiter + _passReturnTXT; 
            Thread.Sleep(100);
            switch (_selectedStateTxt)
            {
                case "CABLE OK":
                    //ce je L1_L1...
                    if((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0]))|| (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6]))|| (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12]))|| (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18]))|| (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    else
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                    break;
                case "L - N CROSSED":
                    //vsi L2_L2.. razen L1_L1 in N_N
                    if ((String.Equals(current_meas, "L2_L2")) || (String.Equals(current_meas, "L3_L3")) || (String.Equals(current_meas, "PE_PE")))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    //L1_N in N_L1
                    else if ((String.Equals(current_meas, "L1_N")) || (String.Equals(current_meas,"N_L1")))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    else
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                    break;
                case "N - OPEN":
                    //ce je L1_L1... razen N_N
                    if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    else
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                    break;
                case "L1 - N SHORTED":
                    //ce je L1_L1...
                    if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    //L1_N in N_L1
                    else if ((String.Equals(current_meas, "L1_N")) || (String.Equals(current_meas, "N_L1")))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    else
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                    break;
                case "L1 - PE SHORTED":
                    //ce je L1_L1...
                    if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24]))) Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    //L1_N in N_L1
                    else if ((String.Equals(current_meas, "L1_PE")) || (String.Equals(current_meas, "PE_L1")))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    else
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                    break;
                case "L1 - L3 CROSSED":
                    //ce je L2_L2, N_N, PE_PE
                    if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    //L1_N in N_L1
                    else if ((String.Equals(current_meas, "L1_L3")) || (String.Equals(current_meas, "L3_L1")))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    else
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                    break;
                case "1P_L - N SHORTED":
                    //ce je L1_L1...
                    if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24]))) Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    //L1_N in N_L1
                    else if ((String.Equals(current_meas, "L1_N")) || (String.Equals(current_meas, "N_L1")))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    else
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                    break;
                case "1P_L OPEN":
                    //ce je L1_L1...
                    if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24]))) Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    else
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                    break;
                case "L1-L2, L3-N CROSSED":
                    //ce je L1_L1...
                    if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    else if((String.Equals(current_meas, "L1_L2")) || (String.Equals(current_meas, "L2_L1")) || (String.Equals(current_meas, "L3_N")) || (String.Equals(current_meas, "N_L3")))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    else
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                    break;
                case "PE-L1, L2-N SHORTED":
                    //ce je L1_L1...
                    if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    else if ((String.Equals(current_meas, "PE_L1")) || (String.Equals(current_meas, "L1_PE")) || (String.Equals(current_meas, "L2_N")) || (String.Equals(current_meas, "N_L2")))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    else
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                    break;
                case "ALL CROSSED":
                    //ce je L1_L1...
                    if ((String.Equals(current_meas, "L1_L2"))|| (String.Equals(current_meas, "L2_L3")) || (String.Equals(current_meas, "L3_N")) || (String.Equals(current_meas, "N_PE"))  || (String.Equals(current_meas, "PE_L1")) )
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    else
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                    break;
                case "PE-L1 CROSSED AND OPEN":
                    //ce je L1_L1...
                    if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    else if ((String.Equals(current_meas, "L1_PE")))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    else
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                    break;
                default: break;
            }
            task1_ex = false;
        }

        private void Cable_state_comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            _selectedStateTxt = Cable_state_comboBox.Text;
        }

        private void Cable_type_comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            _selectedTypeTxt = Cable_type_comboBox.Text;
        }

        private void resistance_return_value_TextChanged(object sender, EventArgs e)
        {
            _passReturnTXT = resistance_return_value.Text;
        }

        private void fail_return_textbox_TextChanged(object sender, EventArgs e)
        {
            _failReturnTXT = fail_return_textbox.Text;
        }

        delegate void write_to_test_start_button_Callback(string message);

        private void write_to_test_start_button(string message)
        {
            if (this.polarity_test_start_button.InvokeRequired)
            {
                write_to_test_start_button_Callback d = new write_to_test_start_button_Callback(write_to_test_start_button);
                this.Invoke(d, new object[] { message });
            }
            else
            {
                this.polarity_test_start_button.Text = message;
                //this.terminal_textbox_send.AppendText(Environment.NewLine);
            }
        }
    }
}
