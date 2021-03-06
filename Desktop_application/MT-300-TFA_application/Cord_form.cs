﻿using System;
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
        private Form1 MainForm;
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
        private bool _cordAllToPEState = false;
        private bool _cordOneToPEState = false;
        private bool _cordPhaseToPhaseState = false;

        public Cord_form(serial_com S_object, Form1 main_form)
        {
            InitializeComponent();
            Serial_object = S_object;
            MainForm = main_form;
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
            CwLimitTextbox.AppendText("10.0");
            ContLimitTextbox.AppendText("1.0");
            RisoLimitTextbox.AppendText("1.0");
            return_iso_ok_textbox.AppendText("10.0");
            return_iso_nok_textbox.AppendText("0.5");
            iso_state_combobox.SelectedIndex = 0;
            fail_return_textbox.AppendText("15.0");
            _failReturnTXT = fail_return_textbox.Text;
            _passReturnTXT = resistance_return_value.Text;
            //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
            //                    Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
            //                    serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[2], "", "");
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
                //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                //    Settings1.Default._ID_TFA, serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[1], "","");
                Serial_object.transmittComand((int)serial_com.funcEnums.__C_W, (int)serial_com.commandEnums.__STOP, (int)serial_com.stdEnums.__CORD, "", "");
                _cordInitiated = false;
                _cordCorrectWitingInProg = false;
                _cordCorrectWitingMeasured = false;
                _cordContinuityInProg = false;
                _cordContinuityMeasured = false;
                _cordAllToPEInProg = false;
                _cordAllToPEMeasured = false;
                _cordOneToPEInProg = false;
                _cordOneToPEMeasured = false;
                _cordPhaseToPhaseInProg = false;
                _cordPhaseToPhaseMeasured = false;
                _cordAllToPEState = false;
                _cordOneToPEState = false;
                _cordPhaseToPhaseState = false;
                mainThread.Abort();
                //set_stop_cord();
            }
        }

        private void cord_sim_functin()
        {
            int cord_sim_count = 0;
            string temp_str="";
            string temp_str2 = "";
            do
            {
                Thread.Sleep(10);
                switch (cord_sim_count)
                {
                    case 0:
                        //_cordInitiated = false;
                        _cordCorrectWitingInProg = false;
                        _cordCorrectWitingMeasured = false;
                        _cordContinuityInProg = false;
                        _cordContinuityMeasured = false;
                        _cordAllToPEInProg = false;
                        _cordAllToPEMeasured = false;
                        _cordOneToPEInProg = false;
                       _cordOneToPEMeasured = false;
                        _cordPhaseToPhaseInProg = false;
                        _cordPhaseToPhaseMeasured = false;
                        _cordAllToPEState = false;
                        _cordOneToPEState = false;
                        _cordPhaseToPhaseState = false;

                        if (String.Equals(getCableTypeComboValue(), "1 PHASE"))
                            temp_str=Serial_object.buildPhaseStr(true);
                        else
                            temp_str=Serial_object.buildPhaseStr(false);
                        cord_sim_count++;
                        break;
                    case 1://C_W
                            temp_str2 = String.Format("{0},LIMIT|{1}", temp_str,CwLimitTextbox.Text);
                            //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                            //    Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                            //    serial_com.FUNCTION_COMMUNICATON_NAMES[0], serial_com.CORD_CODE_NAMES[10], "", "");
                            Serial_object.transmittComand((int)serial_com.funcEnums.__C_W,(int)serial_com.commandEnums.__START,(int)serial_com.stdEnums.__CORD,temp_str2,"");
                            cord_sim_count++;
                            _cordCorrectWitingInProg = true;
                        break;
                    case 2://CONT
                        if (_cordCorrectWitingMeasured)
                        {
                            temp_str2 = String.Format("{0},LIMIT|{1}", temp_str, CwLimitTextbox.Text);
                            Serial_object.transmittComand((int)serial_com.funcEnums.__CONT, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__CORD, temp_str2, "");
                            cord_sim_count++;
                            _cordContinuityInProg = true;
                        }
                        break;
                    case 3://ALL-PE
                        if (_cordContinuityMeasured)
                        {
                            temp_str2 = String.Format("{0},LIMIT|{1}", temp_str, CwLimitTextbox.Text);
                            Serial_object.transmittComand((int)serial_com.funcEnums.__ALL_PE, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__CORD, temp_str2, "");
                            cord_sim_count++;
                            _cordAllToPEInProg = true;
                        }
                        break;
                    case 4:
                        if (_cordAllToPEMeasured)
                        {
                            if (_cordAllToPEState)
                            {
                                temp_str2 = String.Format("{0},LIMIT|{1}", temp_str, CwLimitTextbox.Text);
                                Serial_object.transmittComand((int)serial_com.funcEnums.__PH_PH, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__CORD, temp_str2, "");
                                cord_sim_count=6;
                                _cordPhaseToPhaseInProg = true;
                            }
                            else
                            {
                                temp_str2 = String.Format("{0},LIMIT|{1}", temp_str, CwLimitTextbox.Text);
                                Serial_object.transmittComand((int)serial_com.funcEnums.__ONE_PE, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__CORD, temp_str2, "");
                                cord_sim_count++;
                                _cordOneToPEInProg = true;
                            }

                        }
                        break;
                    case 5:
                        if (_cordOneToPEMeasured)
                        {
                            temp_str2 = String.Format("{0},LIMIT|{1}", temp_str, CwLimitTextbox.Text);
                            Serial_object.transmittComand((int)serial_com.funcEnums.__PH_PH, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__CORD, temp_str2, "");
                            cord_sim_count++;
                            _cordPhaseToPhaseInProg = true;
                        }
                        break;
                    case 6:
                        if (_cordPhaseToPhaseMeasured)
                        {
                            set_stop_cord();
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
            _cordCorrectWitingInProg = false;
            _cordCorrectWitingMeasured = false;
            _cordContinuityInProg = false;
            _cordContinuityMeasured = false;
            _cordAllToPEInProg = false;
            _cordAllToPEMeasured = false;
            _cordOneToPEInProg = false;
            _cordOneToPEMeasured = false;
            _cordPhaseToPhaseInProg = false;
            _cordPhaseToPhaseMeasured = false;
            _cordAllToPEState = false;
            _cordOneToPEState = false;
            _cordPhaseToPhaseState = false;
        }
        

        public void OnCordReturnResult(object sender, string returned_string, string command, string leftover)
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
            else if (String.Equals(command, serial_com.FUNCTION_COMMUNICATON_NAMES[5]))//RPE
            {
                if (String.Equals(returned_string, serial_com.COMMAND_COMMUNICATON_NAMES[0]))
                {
                    cord_return_RPE_result(leftover);
                }
                else if (String.Equals(returned_string, serial_com.COMMAND_COMMUNICATON_NAMES[2]))
                {
                    cord_return_RPE_result(leftover);
                }
            }
            else if (String.Equals(command, serial_com.FUNCTION_COMMUNICATON_NAMES[6]))//RISO
            {
                if (String.Equals(returned_string, serial_com.COMMAND_COMMUNICATON_NAMES[0]))
                {
                    cord_return_RISO_result(leftover);
                }
                else if (String.Equals(returned_string, serial_com.COMMAND_COMMUNICATON_NAMES[2]))
                {
                    cord_return_RISO_result(leftover);
                }
            }
            else if (String.Equals(command, serial_com.FUNCTION_COMMUNICATON_NAMES[0])) //CW
            {
                if (String.Equals(returned_string, serial_com.COMMAND_COMMUNICATON_NAMES2[1])) //RESULT
                {
                    _cordCorrectWitingMeasured = true;
                    _cordCorrectWitingInProg = false;
                }
            }
            else if (String.Equals(command, serial_com.FUNCTION_COMMUNICATON_NAMES[1])) //CONT
            {
                if (String.Equals(returned_string, serial_com.COMMAND_COMMUNICATON_NAMES2[1])) //RESULT
                {
                    _cordContinuityMeasured = true;
                    _cordContinuityInProg = false;
                }
            }
            else if (String.Equals(command, serial_com.FUNCTION_COMMUNICATON_NAMES[3])) //ONE-PE
            {
                if (String.Equals(returned_string, serial_com.COMMAND_COMMUNICATON_NAMES2[1])) //RESULT
                {
                    _cordOneToPEMeasured = true;
                    _cordOneToPEInProg = false;
                }
            }
            else if (String.Equals(command, serial_com.FUNCTION_COMMUNICATON_NAMES[4])) //PH-PH
            {
                if (String.Equals(returned_string, serial_com.COMMAND_COMMUNICATON_NAMES2[1])) //RESULT
                {
                    _cordPhaseToPhaseMeasured = true;
                    _cordPhaseToPhaseInProg = false;
                }
            }
            else if (String.Equals(command, serial_com.FUNCTION_COMMUNICATON_NAMES[7])) //EVENT
            {
                if (String.Equals(returned_string, serial_com.EVENT_COMMUNICATON_NAMES[0])) //STOPPED
                {
                    set_stop_cord();
                }
            }
            
            //else if (String.Equals(command, serial_com.CORD_CODE_NAMES[7])) //RISO_ALL-PE
            //{
            //    if (String.Equals(returned_string, "PASS"))
            //        _cordAllToPEState = true;
            //    else if (String.Equals(returned_string, "FAIL"))
            //        _cordAllToPEState = false;
            //    _cordAllToPEMeasured = true;
            //    _cordAllToPEInProg = false;
            //}
            //else if (String.Equals(command, serial_com.CORD_CODE_NAMES[8])) //RISO_ONE-PE
            //{
            //    if (String.Equals(returned_string, "PASS"))
            //        _cordOneToPEState = true;
            //    else if (String.Equals(returned_string, "FAIL"))
            //        _cordOneToPEState = false;
            //    _cordOneToPEMeasured = true;
            //    _cordOneToPEInProg = false;
            //}
            //else if (String.Equals(command, serial_com.CORD_CODE_NAMES[9])) //RISO_PH-PH
            //{
            //    if (String.Equals(returned_string, "PASS"))
            //        _cordPhaseToPhaseState = true;
            //    else if (String.Equals(returned_string, "FAIL"))
            //        _cordPhaseToPhaseState = false;
            //    _cordPhaseToPhaseMeasured = true;
            //    _cordPhaseToPhaseInProg = false;
            //}
            //else if (String.Equals(command, serial_com.CORD_CODE_NAMES[11])) //CW result
            //{
            //    _cordCorrectWitingMeasured = true;
            //    _cordCorrectWitingInProg = false;
            //}
            //else if (String.Equals(command, serial_com.CORD_CODE_NAMES[13])) //CONT result
            //{
            //    _cordContinuityMeasured = true;
            //    _cordContinuityInProg = false;
            //}
            //else if (String.Equals(command, serial_com.CORD_CODE_NAMES[17])) //get RISO resistance
            //{
            //    cord_return_RISO_result(returned_string);
            //}
            //else if (String.Equals(command, serial_com.CORD_CODE_NAMES[19])) //start RPE L START
            //{
            //    Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
            //        Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
            //        serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[20], "", "");
            //}
            //else if (String.Equals(command, serial_com.CORD_CODE_NAMES[21])) //start RPE H START
            //{
            //    Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
            //        Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
            //        serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[22], "", "");
            //}
            //else if (String.Equals(command, serial_com.CORD_CODE_NAMES[25])) //start RPE STOP
            //{
            //    Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
            //        Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
            //        serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[26], "", "");
            //}
            //else if (String.Equals(command, serial_com.CORD_CODE_NAMES[23])) //start RISO START
            //{
            //    Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
            //        Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
            //        serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[24], "", "");
            //}
            //else if (String.Equals(command, serial_com.CORD_CODE_NAMES[27])) //start RISO STOP
            //{
            //    Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
            //        Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
            //        serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[28], "", "");
            //}
            //else if (String.Equals(command, serial_com.CORD_CODE_NAMES[30])) //CORD deinitiated
            //{

            //}

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
                if (_cordAllToPEInProg || _cordOneToPEInProg ||_cordPhaseToPhaseInProg)
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
            Thread.Sleep(100);
            if (_cordAllToPEInProg)
            {
                switch (getCableInsulationComboValue())
                {
                    case "L1 - PE":
                        _cordAllToPEState = false;//temp_str = _failTempString;
                        break;
                    case "L3 - PE":
                        _cordAllToPEState = false;//temp_str = _failTempString;
                        break;
                    case "L1,N -PE":
                        _cordAllToPEState = false;//temp_str = _failTempString;
                        break;
                    case "N - PE":
                        _cordAllToPEState = false;//temp_str = _failTempString;
                        break;
                    default:
                        _cordAllToPEState = true;//temp_str = _passTempString;
                        break;
                }
                _cordAllToPEMeasured = true;
                _cordAllToPEInProg = false;
                temp_str = "";
            }
            else if (_cordOneToPEInProg)
            {
                switch (getCableInsulationComboValue())
                {
                    case "L1 - PE":
                        if (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1]))
                            temp_str = _passTempString;
                        else
                            temp_str = _failTempString; 
                        break;
                    case "L3 - PE":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1]))||(String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[2])))
                            temp_str = _failTempString;
                        else
                            temp_str = _passTempString; 
                        break;
                    case "L1,N -PE":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1]))||(String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[2]))||(String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[3]))||(String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[6])))
                            temp_str = _failTempString;
                        else
                            temp_str = _passTempString; 
                        break;
                    case "N - PE":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1]))||(String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[2]))||(String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[3])))
                            temp_str = _failTempString;
                        else
                            temp_str = _passTempString; 
                        break;
                    default:
                        temp_str = _passTempString;
                        break;
                }
            }
            else if (_cordPhaseToPhaseInProg)
            {
                switch (getCableInsulationComboValue())
                {
                     case "L1 - N":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[7])))
                            temp_str = _failTempString;
                        else
                            temp_str = _passTempString; 
                        break;
                    case "L2,L3 - N":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[7])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[8])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[9])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[10])))
                            temp_str = _failTempString;
                        else
                            temp_str = _passTempString; 
                        break;
                    case "L1 - L2":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[15])))
                            temp_str = _failTempString;
                        else
                            temp_str = _passTempString; 
                        break;
                    default:
                        temp_str = _passTempString;
                        break;
                }
            }
            else
            {
                temp_str = "";
            }
            if (!_cordAllToPEInProg)
                Serial_object.returnRisoRes(temp_str);
            //Serial_object.Send_protocol_message(
            //                    Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
            //                    Settings1.Default._ID_TFA,serial_com.COMMAND_TYPE_NAMES[5],
            //                    serial_com.CORD_CODE_NAMES[18], temp_str, "");
        }

        private void cord_get_RPE_task()
        {
            int current_msg_id = 0;
            string failTempString;
            string passTempString;
            string temp_string;
            if (_cordCorrectWitingInProg)
            {
                failTempString = _failReturnTXT;
                passTempString = _passReturnTXT;
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
                            temp_string = passTempString;
                        else
                            temp_string = failTempString;
                        break;
                    case "L - N CROSSED":
                        //vsi L2_L2.. razen L1_L1 in N_N
                        if ((String.Equals(current_meas, "L2_L2")) || (String.Equals(current_meas, "L3_L3")) ||
                            (String.Equals(current_meas, "PE_PE")))
                             temp_string = passTempString;
                            //L1_N in N_L1
                        else if ((String.Equals(current_meas, "L1_N")) || (String.Equals(current_meas, "N_L1")))
                             temp_string = passTempString;
                        else
                            temp_string = failTempString;
                        break;
                    case "N - OPEN":
                        //ce je L1_L1... razen N_N
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                             temp_string = passTempString;
                        else
                            temp_string = failTempString;
                        break;
                    case "L1 - N SHORTED":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                             temp_string = passTempString;
                            //L1_N in N_L1
                        else if ((String.Equals(current_meas, "L1_N")) || (String.Equals(current_meas, "N_L1")))
                             temp_string = passTempString;
                        else
                            temp_string = failTempString;
                        break;
                    case "L1 - PE SHORTED":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                             temp_string = passTempString;
                            //L1_N in N_L1
                        else if ((String.Equals(current_meas, "L1_PE")) || (String.Equals(current_meas, "PE_L1")))
                             temp_string = passTempString;
                        else
                            temp_string = failTempString;
                        break;
                    case "L1 - L3 CROSSED":
                        //ce je L2_L2, N_N, PE_PE
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                             temp_string = passTempString;
                            //L1_N in N_L1
                        else if ((String.Equals(current_meas, "L1_L3")) || (String.Equals(current_meas, "L3_L1")))
                             temp_string = passTempString;
                        else
                            temp_string = failTempString;
                        break;
                    case "1P_L - N SHORTED":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                             temp_string = passTempString;
                            //L1_N in N_L1
                        else if ((String.Equals(current_meas, "L1_N")) || (String.Equals(current_meas, "N_L1")))
                             temp_string = passTempString;
                        else
                            temp_string = failTempString;

                        break;
                    case "1P_L OPEN":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                             temp_string = passTempString;
                        else
                            temp_string = failTempString;
                        break;
                    case "L1-L2, L3-N CROSSED":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                             temp_string = passTempString;
                        else if ((String.Equals(current_meas, "L1_L2")) || (String.Equals(current_meas, "L2_L1")) ||
                                 (String.Equals(current_meas, "L3_N")) || (String.Equals(current_meas, "N_L3")))
                             temp_string = passTempString;
                        else
                            temp_string = failTempString;
                        break;
                    case "PE-L1, L2-N SHORTED":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[0])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                             temp_string = passTempString;
                        else if ((String.Equals(current_meas, "PE_L1")) || (String.Equals(current_meas, "L1_PE")) ||
                                 (String.Equals(current_meas, "L2_N")) || (String.Equals(current_meas, "N_L2")))
                             temp_string = passTempString;
                        else
                            temp_string = failTempString;
                        break;
                    case "ALL CROSSED":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, "L1_L2")) || (String.Equals(current_meas, "L2_L3")) ||
                            (String.Equals(current_meas, "L3_N")) || (String.Equals(current_meas, "N_PE")) ||
                            (String.Equals(current_meas, "PE_L1")))
                            temp_string = passTempString;
                        else
                            temp_string = failTempString;
                        break;
                    case "PE-L1 CROSSED AND OPEN":
                        //ce je L1_L1...
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[6])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[12])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[18])) ||
                            (String.Equals(current_meas, serial_com.CORD_LEFTOVER_NAMES[24])))
                            temp_string = passTempString;
                        else if ((String.Equals(current_meas, "L1_PE")))
                            temp_string = passTempString;
                        else
                            temp_string = failTempString;
                        break;
                    default:
                        temp_string = "0.00";
                        break;
                }
                Serial_object.returnRpeRes(temp_string);
                //current_msg_id = Serial_object.Send_protocol_message(
                //                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                //                Settings1.Default._ID_TFA, serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[4], temp_string, "");
            }
            else if (_cordContinuityInProg)
            {
                Thread.Sleep(100);
               string temp_str;
               if(String.Equals(current_meas, "L1_L1"))
               {
                   temp_str = getContL1TxtValue();
               }
               else if (String.Equals(current_meas, "L2_L2"))
               {
                   temp_str = getContL2TxtValue();
               }
               else if (String.Equals(current_meas, "L3_L3"))
               {
                   temp_str = getContL3TxtValue();
               }
               else if (String.Equals(current_meas, "N_N"))
               {
                   temp_str = getContNTxtValue();
               }
               else if (String.Equals(current_meas, "PE_PE"))
               {
                   temp_str = getContPETxtValue();
               }
               else
                   temp_str = "0.00";
               Serial_object.returnRpeRes(temp_str);
                //Serial_object.Send_protocol_message(
                //  Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                //  Settings1.Default._ID_TFA, serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[4], temp_str, "");
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

        private void Cord_form_Load(object sender, EventArgs e)
        {

        }

        private void Cord_form_FormClosing(object sender, FormClosingEventArgs e)
        {
            //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
            //                    Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
            //                    serial_com.COMMAND_TYPE_NAMES[5], serial_com.CORD_CODE_NAMES[31], "", "");
            MainForm.Enable_buttons();
        }

    }
}
