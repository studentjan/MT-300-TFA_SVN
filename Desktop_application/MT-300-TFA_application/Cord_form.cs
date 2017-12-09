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
        private Thread mainThread;
        private string current_meas;
        private bool task1_ex;
        private string _selectedStateTxt;
        private string _selectedTypeTxt;
        private string _failReturnTXT;
        private string _passReturnTXT;
        private string delimiter = "|";
        private string prev_msg = "";
        
        private bool _cordInitiated = false;
        private bool _cordCorrectWitingInProg = false;
        private bool _cordCorrectWitingMeasured = false;
        private bool _cordContinuityInProg = false;
        private bool _cordContinuityMeasured = false;
        private bool _cordAllToPEInProg = false;
        private bool _cordAllToPEMeasured = false;
        private bool _cordOneToPEInProg = false;
        private bool _cordOneToPEMeasured = false;
        private bool _cordPhaseToPhaseInProg = false;
        private bool _cordPhaseToPhaseMeasured = false;

        public Cord_form(serial_com S_object)
        {
            InitializeComponent();
            Serial_object = S_object;
            Serial_object.cordReturnEventHandler += new serial_com.cord_return_result(OnCordReturnResult);
            Cable_state_comboBox.SelectedIndex = 0;
            Cable_type_comboBox.SelectedIndex = 0;
            _selectedStateTxt = Cable_state_comboBox.Text;
            _selectedTypeTxt = Cable_type_comboBox.Text;
            resistance_return_value.AppendText("0.2");
            cont_L1_textbox.AppendText("0.2");
            cont_L2_textbox.AppendText("0.2");
            cont_L3_textbox.AppendText("0.2");
            cont_N_textbox.AppendText("0.2");
            cont_PE_textbox.AppendText("0.2");
            return_iso_ok_textbox.AppendText("10.0");
            return_iso_nok_textbox.AppendText("0.5");
            iso_state_combobox.SelectedIndex = 0;
            fail_return_textbox.AppendText("5.0");
            _failReturnTXT = fail_return_textbox.Text;
            _passReturnTXT = resistance_return_value.Text;
        }

        private void polarity_test_start_button_Click(object sender, EventArgs e)
        {
            //start
            if (CordMeasInProg == false)
            {
                CordMeasInProg = true;
                meas_in_progress_ind.BackColor = Color.Red;
                polarity_test_start_button.Text = "STOP";
                mainThread = new Thread(cord_sim_functin);
                mainThread.Start();
                //zazenemo meritev
            }
            //stop
            else
            {
                //ustavimo meritev
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                    Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                    serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[1], "");
                //set_stop_cord();
            }
        }

        private void cord_sim_functin()
        {
            int cord_sim_count = 0;
            
            do
            {
                Thread.Sleep(10);
                switch (cord_sim_count)
                {
                    case 0:
                        if (String.Equals(getCableTypeComboValue(), "1 PHASE"))
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[2],
                                serial_com.CORD_LEFTOVER_NAMES[25]);
                        else if (String.Equals(getCableTypeComboValue(), "3 PHASE"))
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[2],
                                serial_com.CORD_LEFTOVER_NAMES[26]);
                        cord_sim_count++;
                        break;
                    case 1:
                        if (_cordInitiated)
                        {
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[10],
                                "");
                            cord_sim_count++;
                            _cordCorrectWitingInProg = true;
                        }
                        break;
                    case 2:
                        if (_cordCorrectWitingMeasured)
                        {
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[12],
                                "");
                            cord_sim_count++;
                            _cordContinuityInProg = true;
                        }
                        break;
                    case 3:
                        if (_cordContinuityMeasured)
                        {
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[16],
                                "");
                            cord_sim_count++;
                            _cordAllToPEInProg = true;
                        }
                        break;
                    case 4:
                        if (_cordAllToPEMeasured)
                        {
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[17],
                                "");
                            cord_sim_count++;
                            _cordOneToPEInProg = true;
                        }
                        break;
                    case 5:
                        if (_cordOneToPEMeasured)
                        {
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[18],
                                "");
                            cord_sim_count++;
                            _cordPhaseToPhaseInProg = true;
                        }
                        break;
                    case 6:
                        if (_cordPhaseToPhaseMeasured)
                        {
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[1],        //stop
                                "");
                            cord_sim_count++;
                        }
                        break;
                }
            } while (cord_sim_count < 7);
        }

        private void set_stop_cord()
        {
            meas_in_progress_ind.BackColor = Color.Empty;
            write_to_test_start_button("START");
            CordMeasInProg = false;
        }
        

        public void OnCordReturnResult(object sender, string returned_string, string command)
        {
            if (String.Equals(command, serial_com.CORD_CODE_NAMES[3])) //get RPE resistance
            {
                cord_return_RPE_result(returned_string);
            }
            else if (String.Equals(command, serial_com.CORD_CODE_NAMES[5])) //stopped
            {
                set_stop_cord();
            }
            else if (String.Equals(command, serial_com.CORD_CODE_NAMES[6])) //initiated
            {
                _cordInitiated = true;
            }
            else if (String.Equals(command, serial_com.CORD_CODE_NAMES[7])) //RISO_ALL-PE
            {
                _cordAllToPEMeasured = true;
                _cordAllToPEInProg = false;
            }
            else if (String.Equals(command, serial_com.CORD_CODE_NAMES[8])) //RISO_ONE-PE
            {
                _cordOneToPEMeasured = true;
                _cordOneToPEInProg = false;
            }
            else if (String.Equals(command, serial_com.CORD_CODE_NAMES[9])) //RISO_PH-PH
            {
                _cordPhaseToPhaseMeasured = true;
                _cordPhaseToPhaseInProg = false;
            }
            else if (String.Equals(command, serial_com.CORD_CODE_NAMES[11])) //CW result
            {
                _cordCorrectWitingMeasured = true;
                _cordCorrectWitingInProg = false;
            }
            else if (String.Equals(command, serial_com.CORD_CODE_NAMES[13])) //CONT result
            {
                _cordContinuityMeasured = true;
                _cordContinuityInProg = false;
            }
            else if (String.Equals(command, serial_com.CORD_CODE_NAMES[3])) //get RISO resistance
            {
                cord_return_RISO_result(returned_string);
            }
            else if (String.Equals(command, serial_com.CORD_CODE_NAMES[20])) //start RPE L...
            {
                
            }
            else if (String.Equals(command, serial_com.CORD_CODE_NAMES[3])) //get RISO resistance
            {
                
            }
            
        }

        public void cord_return_RPE_result(string meas_con)
        {
            if (!String.Equals(prev_msg, meas_con))
            {
                //if (task1_ex == false)
                //{
                if ((_cordCorrectWitingInProg) || (_cordContinuityInProg))
                {
                    prev_msg = meas_con;
                    //task1_ex = true;
                    task1 = new Thread(cord_get_RPE_task);
                    task1.Start();
                    current_meas = meas_con;
                }
                //}
            }
        }
        public void cord_return_RISO_result(string meas_con)
        {
                if (_cordCorrectWitingInProg)
                {
                    prev_msg = meas_con;
                    //task1_ex = true;
                    task1 = new Thread(cord_get_RISO_task);
                    task1.Start();
                    current_meas = meas_con;
                }
        }

        private void cord_get_RISO_task()
        {
            string temp_str;
            string _failTempString = getIsoNOKValue();
            string _passTempString = getIsoOKValue();
            if (_cordAllToPEInProg)
            {
                switch (getCableInsulationComboValue())
                {
                    case "L1 - PE":
                        temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _failTempString;
                        break;
                    case "L3 - PE":
                        temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _failTempString;
                        break;
                    case "L1,N - PE":
                        temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _failTempString;
                        break;
                    case "N - PE":
                        temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _failTempString;
                        break;
                    default:
                        temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _passTempString;
                        break;
                }
            }
            else if (_cordOneToPEInProg)
            {
                switch (getCableInsulationComboValue())
                {
                    case "L1 - PE":
                        if (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1]))
                            temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _passTempString;
                        else
                            temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _failTempString; 
                        break;
                    case "L3 - PE":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1]))||(String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[2])))
                            temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _failTempString;
                        else
                            temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _passTempString; 
                        break;
                    case "L1,N - PE":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1]))||(String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[2]))||(String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[3]))||(String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[6])))
                            temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _failTempString;
                        else
                            temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _passTempString; 
                        break;
                    case "N - PE":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1]))||(String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[2]))||(String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[3])))
                            temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _failTempString;
                        else
                            temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _passTempString; 
                        break;
                    default:
                        temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _passTempString;
                        break;
                }
            }
            else if (_cordPhaseToPhaseInProg)
            {
                switch (getCableInsulationComboValue())
                {
                     case "L1 - N":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[7])))
                            temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _failTempString;
                        else
                            temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _passTempString; 
                        break;
                    case "L2,L3 - N":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[7]))||(String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[8]))||(String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[9])))
                            temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _failTempString;
                        else
                            temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _passTempString; 
                        break;
                    case "L1 - L2":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[15])))
                            temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _failTempString;
                        else
                            temp_str = serial_com.CORD_CODE_NAMES[18] + delimiter + _passTempString; 
                        break;
                    default:
                        temp_str = serial_com.CORD_CODE_NAMES[8] + delimiter + _passTempString;
                        break;
                }
            }
            else
            {
                temp_str = "";
            }
            Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], temp_str, "");
        }

        private void cord_get_RPE_task()
        {
            int current_msg_id = 0;
            string failTempString;
            string passTempString;
            if (_cordCorrectWitingInProg)
            {
                failTempString = serial_com.CORD_CODE_NAMES[4] + delimiter + _failReturnTXT;
                passTempString = serial_com.CORD_CODE_NAMES[4] + delimiter + _passReturnTXT;
                Thread.Sleep(100);
                switch (_selectedStateTxt)
                {
                    case "CABLE OK":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                        else
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                        break;
                    case "L - N CROSSED":
                        //vsi L2_L2.. razen L1_L1 in N_N
                        if ((String.Equals(current_meas, "L2_L2")) || (String.Equals(current_meas, "L3_L3")) ||
                            (String.Equals(current_meas, "PE_PE")))
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                            //L1_N in N_L1
                        else if ((String.Equals(current_meas, "L1_N")) || (String.Equals(current_meas, "N_L1")))
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                        else
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                        break;
                    case "N - OPEN":
                        //ce je L1_L1... razen N_N
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                        else
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                        break;
                    case "L1 - N SHORTED":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                            //L1_N in N_L1
                        else if ((String.Equals(current_meas, "L1_N")) || (String.Equals(current_meas, "N_L1")))
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                        else
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                        break;
                    case "L1 - PE SHORTED":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5],
                                passTempString,
                                "");
                            //L1_N in N_L1
                        else if ((String.Equals(current_meas, "L1_PE")) || (String.Equals(current_meas, "PE_L1")))
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                        else
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                        break;
                    case "L1 - L3 CROSSED":
                        //ce je L2_L2, N_N, PE_PE
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                            //L1_N in N_L1
                        else if ((String.Equals(current_meas, "L1_L3")) || (String.Equals(current_meas, "L3_L1")))
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                        else
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                        break;
                    case "1P_L - N SHORTED":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5],
                                passTempString,
                                "");
                            //L1_N in N_L1
                        else if ((String.Equals(current_meas, "L1_N")) || (String.Equals(current_meas, "N_L1")))
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                        else
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                        break;
                    case "1P_L OPEN":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5],
                                passTempString,
                                "");
                        else
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                        break;
                    case "L1-L2, L3-N CROSSED":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                        else if ((String.Equals(current_meas, "L1_L2")) || (String.Equals(current_meas, "L2_L1")) ||
                                 (String.Equals(current_meas, "L3_N")) || (String.Equals(current_meas, "N_L3")))
                            current_msg_id =
                                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                    Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                    serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5],
                                    passTempString, "");
                        else
                            current_msg_id =
                                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                    Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                    serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5],
                                    failTempString, "");
                        break;
                    case "PE-L1, L2-N SHORTED":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                        else if ((String.Equals(current_meas, "PE_L1")) || (String.Equals(current_meas, "L1_PE")) ||
                                 (String.Equals(current_meas, "L2_N")) || (String.Equals(current_meas, "N_L2")))
                            current_msg_id =
                                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                    Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                    serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5],
                                    passTempString, "");
                        else
                            current_msg_id =
                                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                    Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                    serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.COMMAND_TYPE_NAMES[5],
                                    failTempString, "");
                        break;
                    case "ALL CROSSED":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, "L1_L2")) || (String.Equals(current_meas, "L2_L3")) ||
                            (String.Equals(current_meas, "L3_N")) || (String.Equals(current_meas, "N_PE")) ||
                            (String.Equals(current_meas, "PE_L1")))
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                        else
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                        break;
                    case "PE-L1 CROSSED AND OPEN":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                        else if ((String.Equals(current_meas, "L1_PE")))
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], passTempString, "");
                        else
                            current_msg_id = Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                                serial_com.COMMAND_TYPE_NAMES[5], failTempString, "");
                        break;
                    default:
                        break;
                }
            }
            else if (_cordContinuityInProg)
            {
               string temp_str;
               if(String.Equals(current_meas, "L1_L1"))
               {
                   temp_str = serial_com.CORD_CODE_NAMES[4] + delimiter + getContL1TxtValue();
               }
               else if (String.Equals(current_meas, "L2_L2"))
               {
                   temp_str = serial_com.CORD_CODE_NAMES[4] + delimiter + getContL2TxtValue();
               }
               else if (String.Equals(current_meas, "L3_L3"))
               {
                   temp_str = serial_com.CORD_CODE_NAMES[4] + delimiter + getContL3TxtValue();
               }
               else if (String.Equals(current_meas, "N_N"))
               {
                   temp_str = serial_com.CORD_CODE_NAMES[4] + delimiter + getContNTxtValue();
               }
               else if (String.Equals(current_meas, "PE_PE"))
               {
                   temp_str = serial_com.CORD_CODE_NAMES[4] + delimiter + getContPETxtValue();
               }
               else
                   temp_str = "0.00";
               Serial_object.Send_protocol_message(
                  Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                  Settings1.Default._ID_TFA, serial_com.FUNCTION_COMMUNICATON_NAMES[0],
                  serial_com.COMMAND_TYPE_NAMES[5], temp_str, "");
            }
            task1_ex = false;
        }

        private string getCableStateComboValue()
        {

            string krneki = null;
            //this.Invoke(() => krneki = Cable_state_comboBox.Text);
            this.Invoke(new MethodInvoker(delegate() { krneki = Cable_state_comboBox.Text; }));
            return krneki;
        }
        private string getCableTypeComboValue()
        {
            string krneki = null;
            this.Invoke(new MethodInvoker(delegate() { krneki = Cable_type_comboBox.Text; }));
            return krneki;
        }
        private string getCableInsulationComboValue()
        {
            string krneki = null;
            this.Invoke(new MethodInvoker(delegate() { krneki = iso_state_combobox.Text; }));
            return krneki;
        }
        private string getContL1TxtValue()
        {
            string krneki = null;
            this.Invoke(new MethodInvoker(delegate() { krneki = cont_L1_textbox.Text; }));
            return krneki;
        }
        private string getContL2TxtValue()
        {
            string krneki = null;
            this.Invoke(new MethodInvoker(delegate() { krneki = cont_L2_textbox.Text; }));
            return krneki;
        }
        private string getContL3TxtValue()
        {
            string krneki = null;
            this.Invoke(new MethodInvoker(delegate() { krneki = cont_L3_textbox.Text; }));
            return krneki;
        }
        private string getContNTxtValue()
        {
            string krneki = null;
            this.Invoke(new MethodInvoker(delegate() { krneki = cont_N_textbox.Text; }));
            return krneki;
        }
        private string getContPETxtValue()
        {
            string krneki = null;
            this.Invoke(new MethodInvoker(delegate() { krneki = cont_PE_textbox.Text; }));
            return krneki;
        }
        private string getIsoOKValue()
        {
            string krneki = null;
            this.Invoke(new MethodInvoker(delegate() { krneki = return_iso_ok_textbox.Text; }));
            return krneki;
        }
        private string getIsoNOKValue()
        {
            string krneki = null;
            this.Invoke(new MethodInvoker(delegate() { krneki = return_iso_nok_textbox.Text; }));
            return krneki;
        }

        //private string getCableStateComboValue()
        //{
        //    string krneki = Cable_state_comboBox.Text;
        //    return krneki;
        //}

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
