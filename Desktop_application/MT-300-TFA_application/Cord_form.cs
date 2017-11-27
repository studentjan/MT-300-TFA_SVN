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

        public Cord_form(serial_com S_object)
        {
            InitializeComponent();
            Serial_object = S_object;
            task1 = new Thread(cord_basic_task);
            Cable_state_comboBox.SelectedIndex = 0;
            Cable_type_comboBox.SelectedIndex = 0;
            _selectedStateTxt = Cable_state_comboBox.Text;
            _selectedTypeTxt = Cable_type_comboBox.Text;
            resistance_return_value.AppendText("5.0");
            fail_return_textbox.AppendText("0.2");
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
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[0], "");

            }
            else
            {
                //ustavimo meritev
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[1], "");
                CordMeasInProg = false;
                meas_in_progress_ind.BackColor = Color.Empty;
                polarity_test_start_button.Text = "START";
            }
        }
        public void cord_return_result(string meas_con)
        {
            if (task1_ex == false)
            {
                task1_ex = true;
                task1 = new Thread(cord_basic_task);
                task1.Start();
                current_meas = meas_con;
                
            }
        }
        private void cord_basic_task()
        {
            string failTempString = serial_com.CORD_CODE_NAMES[4] + delimiter + _passReturnTXT;
            string passTempString = serial_com.CORD_CODE_NAMES[4] + delimiter + _failReturnTXT; 
            Thread.Sleep(100);
            switch (_selectedStateTxt)
            {
                case "CABLE OK":
                    Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                    break;
                case "L - N CROSSED":
                    if ((String.Equals(current_meas, "L_N")) || (String.Equals(current_meas, "N_L")))
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                    else
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5],passTempString, "");
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
    }
}
