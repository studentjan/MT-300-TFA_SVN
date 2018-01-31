using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;

namespace MT_300_TFA_application
{
    public partial class Machines_form : Form
    {
        private serial_com Serial_object;
        private Form1 MainForm;
        private Thread task1;
        private Thread task2;
        private Thread task3;

        private int mach_ures_count=0;
        private bool mach_ures_in_prog = false;
        private bool mach_ures_started = false;
        private bool mach_ures_finished = false;

        private bool rpe_in_prog = false;
        private bool riso_all_pe_in_prog = false;
        private bool riso_one_pe_in_prog = false;


        private string prev_msg = "";
        private string current_meas;
        private string delimiter = "|";
        public Machines_form(serial_com S_object, Form1 main_form)
        {
            InitializeComponent();
            return_iso_ok_textbox.AppendText("10.0");
            return_iso_nok_textbox.AppendText("0.5");
            
            Serial_object = S_object;
            reset_analyze_textbox();
            Cable_type_comboBox.SelectedIndex = 0;
            RisoLimitTextbox.AppendText("1.0");
            Serial_object.machReturnEventHandler += OnMachReturnResult;
            Serial_object.analyzeReturnEventHandler += AnalyzeResults;
            iso_state_combobox.SelectedIndex = 0;
            Select_measurement_comboBox.SelectedIndex = 0;
            MainForm = main_form;
        }

        private void test_contactors_button_Click(object sender, EventArgs e)
        {
            string temp_str;
            temp_str = getSetURES();
            //Serial_object.Send_protocol_message(
            //                    Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
            //                    Settings1.Default._ID_TFA, serial_com.COMMAND_TYPE_NAMES[7],
            //                    serial_com.MACH_COMMAND_NAMES[0], temp_str, serial_com.MACH_ADD_NAMES[0]);
            Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.COMMAND_TYPE_NAMES[7],
                                serial_com.MACH_COMMAND_NAMES[10], "", "");
        }

        private string getSetURES()
        {
            string temp_str;
            switch (getMeasSelectedComboValue())
            {
                case "L1-N":
                    temp_str = serial_com.URES_PARAM_NAMES[3];
                    break;
                case "L2-N":
                    temp_str = serial_com.URES_PARAM_NAMES[4];
                    break;
                case "L3-N":
                    temp_str = serial_com.URES_PARAM_NAMES[5];
                    break;
                case "L1-PE":
                    temp_str = serial_com.URES_PARAM_NAMES[0];
                    break;
                case "L2-PE":
                    temp_str = serial_com.URES_PARAM_NAMES[1];
                    break;
                case "L3-PE":
                    temp_str = serial_com.URES_PARAM_NAMES[2];
                    break;
                case "L1-L2":
                    temp_str = serial_com.URES_PARAM_NAMES[6];
                    break;
                case "L2-L3":
                    temp_str = serial_com.URES_PARAM_NAMES[7];
                    break;
                case "L1-L3":
                    temp_str = serial_com.URES_PARAM_NAMES[8];
                    break;
                default:
                    temp_str = "";
                    break;
            }
            return temp_str;
        }

        private void Machines_form_FormClosing(object sender, FormClosingEventArgs e)
        {
            MainForm.Enable_buttons();
        }
        private string getMeasSelectedComboValue()
        {
            string krneki = null;
            this.Invoke(new MethodInvoker(delegate() { krneki = Select_measurement_comboBox.Text; }));
            return krneki;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (mach_ures_in_prog == false)
            {
                string temp_str;
                temp_str = getSetURES();

                Serial_object.transmittComand((int)serial_com.funcEnums.__M_URES, (int)serial_com.commandEnums.__INIT, (int)serial_com.stdEnums.__MACH, "", "");
                mach_ures_count = 0;
                task1 = new Thread(UREStask);
                task1.Start();
                mach_ures_in_prog = true;
            }
            else
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__M_URES, (int)serial_com.commandEnums.__STOP, (int)serial_com.stdEnums.__MACH, "", "");
                clearURESstarted();
            }

        }

        private void setURESstarted()
        {
            //this.Invoke(new MethodInvoker(delegate() { button1.Text = "URES_STOP"; }));
            mach_ures_started = true;

        }
        private void clearURESstarted()
        {
            //this.Invoke(new MethodInvoker(delegate() { button1.Text = "URES_START"; }));
            mach_ures_started = false;
            mach_ures_finished = false;
            mach_ures_in_prog = false;
            mach_ures_count = 0;
            rpe_button.Enabled = true;
            riso_button.Enabled = true;
        }
        private void UREStask()
        {
            do
            {
                Thread.Sleep(200);
                switch (mach_ures_count)
                {
                    case 0:
                        if (mach_ures_started)
                        {
                            Serial_object.transmittComand((int)serial_com.funcEnums.__M_URES, (int)serial_com.commandEnums.__OPEN, (int)serial_com.stdEnums.__MACH, "", "");
                            mach_ures_count++;
                        }
                        break;
                    case 1:
                        if (mach_ures_finished)
                        {
                            mach_ures_count++;
                        }
                        break;
                    case 2:
                        mach_ures_count++;
                        clearURESstarted();
                        break;
                    default:
                        break;
                }
            } while (mach_ures_in_prog);
        }

        private void stop_mach()
        {
            clearURESstarted();
        }

        public void OnMachReturnResult(object sender, string returned_string, string command, string leftover)
        {
            if (String.Equals(command, serial_com.FUNCTION_COMMUNICATON_NAMES[8])) //URES
            {
                if (String.Equals(returned_string, serial_com.MACH_COMMAND_NAMES[9])) //INITIATED
                {
                    setURESstarted();
                }
                if (String.Equals(returned_string, serial_com.COMMAND_COMMUNICATON_NAMES[3])) //OPENED
                {
                    mach_ures_finished = true;
                }
            }
            else if (String.Equals(command, serial_com.FUNCTION_COMMUNICATON_NAMES[2])) //ALL-PE
            {
                if (String.Equals(returned_string, serial_com.COMMAND_COMMUNICATON_NAMES2[1])) //RESULT
                {
                    clearALL_PEStarted();
                }
            }
            else if (String.Equals(command, serial_com.FUNCTION_COMMUNICATON_NAMES[3])) //ONE-PE
            {
                if (String.Equals(returned_string, serial_com.COMMAND_COMMUNICATON_NAMES2[1])) //RESULT
                {
                    clearONE_PEStarted();
                }
            }
            else if (String.Equals(command, serial_com.FUNCTION_COMMUNICATON_NAMES[6]))//RISO
            {
                if (String.Equals(returned_string, serial_com.COMMAND_COMMUNICATON_NAMES[0]))
                {
                    mach_return_RISO_result(leftover);
                }
                else if (String.Equals(returned_string, serial_com.COMMAND_COMMUNICATON_NAMES[2]))
                {
                    mach_return_RISO_result(leftover);
                }
            }
            

        }

        public void mach_return_RISO_result(string meas_con)
        {
            if (riso_all_pe_in_prog || riso_one_pe_in_prog)
            {
                prev_msg = meas_con;
                //task1_ex = true;
                task2 = new Thread(mach_get_RISO_task);
                task2.Start();
                current_meas = meas_con;
            }
        }

        private void mach_get_RISO_task()
        {
            string temp_str;
            string _failTempString = getIsoNOKValue();
            string _passTempString = getIsoOKValue();
            Thread.Sleep(100);
            if (riso_all_pe_in_prog)
            {
                switch (getCableInsulationComboValue())
                {
                    case "L1 - PE":
                        temp_str = _failTempString;
                        break;
                    case "L3 - PE":
                        temp_str = _failTempString;
                        break;
                    case "L1,N -PE":
                        temp_str = _failTempString;
                        break;
                    case "N - PE":
                        temp_str = _failTempString;
                        break;
                    default:
                        temp_str = _passTempString;
                        break;
                }
                clearALL_PEStarted();
            }
            else if (riso_one_pe_in_prog)
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
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[2])))
                            temp_str = _failTempString;
                        else
                            temp_str = _passTempString;
                        break;
                    case "L1,N -PE":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[2])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[3])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[6])))
                            temp_str = _failTempString;
                        else
                            temp_str = _passTempString;
                        break;
                    case "N - PE":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[2])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[3])))
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
            Serial_object.returnRisoRes(temp_str);
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
        private string getCableInsulationComboValue()
        {
            string krneki = null;
            this.Invoke(new MethodInvoker(delegate() { krneki = iso_state_combobox.Text; }));
            return krneki;
        }
        private void setRPEStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Text = "RPE STOP"; }));
            this.Invoke(new MethodInvoker(delegate() { button1.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { test_contactors_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { button1.Enabled = false; }));
        }
        private void clearRPEStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Text = "RPE START"; }));
            this.Invoke(new MethodInvoker(delegate() { button1.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { test_contactors_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { button1.Enabled = true; }));
            rpe_in_prog = false;
        }

        private void setALL_PEStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { riso_button.Text = "STOP ALL-PE"; }));
            this.Invoke(new MethodInvoker(delegate() { button1.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { test_contactors_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { button1.Enabled = false; }));
        }
        private void clearALL_PEStarted()
        {
            if (riso_all_pe_in_prog)
            {
                this.Invoke(new MethodInvoker(delegate() { riso_button.Text = "START ALL-PE"; }));
                this.Invoke(new MethodInvoker(delegate() { button1.Enabled = true; }));
                this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = true; }));
                this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = true; }));
                this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = true; }));
                this.Invoke(new MethodInvoker(delegate() { test_contactors_button.Enabled = true; }));
                this.Invoke(new MethodInvoker(delegate() { button1.Enabled = true; }));
            }
            riso_all_pe_in_prog = false;
        }

        private void setONE_PEStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Text = "STOP ONE-PE"; }));
            this.Invoke(new MethodInvoker(delegate() { button1.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { test_contactors_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { button1.Enabled = false; }));
        }
        private void clearONE_PEStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Text = "START ONE-PE"; }));
            this.Invoke(new MethodInvoker(delegate() { button1.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { test_contactors_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { button1.Enabled = true; }));
            riso_one_pe_in_prog = false;
        }
        private void setAnalyzeStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Text = "STOP MAINS ANALYZE"; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { test_contactors_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { button1.Enabled = false; }));
        }
        private void clearAnalyzeStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Text = "START MAINS ANALYZE"; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { test_contactors_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { button1.Enabled = true; }));
            analyze_in_prog = false;
        }
        private void rpe_button_Click(object sender, EventArgs e)
        {
            if (rpe_in_prog == false)
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__M_RPE, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__MACH, "", "");
                rpe_in_prog = true;
            }
            else
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__M_RPE, (int)serial_com.commandEnums.__STOP, (int)serial_com.stdEnums.__MACH, "", "");
                clearURESstarted();
                rpe_in_prog = false;
            }

        }

        private void riso_button_Click(object sender, EventArgs e)
        {
            
            string temp_str;
            string temp_str2;
            if (riso_all_pe_in_prog == false)
            {
                if (String.Equals(getCableTypeComboValue(), "1 PHASE"))
                    temp_str = Serial_object.buildPhaseStr(true);
                else
                    temp_str = Serial_object.buildPhaseStr(false);
                temp_str2 = String.Format("{0},LIMIT|{1}", temp_str, RisoLimitTextbox.Text);
                Serial_object.transmittComand((int)serial_com.funcEnums.__ALL_PE, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__MACH, temp_str2, "");
                riso_all_pe_in_prog = true;
                setALL_PEStarted();
            }
            else
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__ALL_PE, (int)serial_com.commandEnums.__STOP, (int)serial_com.stdEnums.__MACH, "", "");
                riso_all_pe_in_prog = false;
                clearALL_PEStarted();
            }
        }

        private void riso_button2_Click(object sender, EventArgs e)
        {
            string temp_str;
            string temp_str2;
            if (riso_one_pe_in_prog == false)
            {
                if (String.Equals(getCableTypeComboValue(), "1 PHASE"))
                    temp_str = Serial_object.buildPhaseStr(true);
                else
                    temp_str = Serial_object.buildPhaseStr(false);
                temp_str2 = String.Format("{0},LIMIT|{1}", temp_str, RisoLimitTextbox.Text);
                Serial_object.transmittComand((int)serial_com.funcEnums.__ONE_PE, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__MACH, temp_str2, "");
                riso_one_pe_in_prog = true;
                setONE_PEStarted();
            }
            else
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__ONE_PE, (int)serial_com.commandEnums.__STOP, (int)serial_com.stdEnums.__MACH, "", "");
                //clearURESstarted();
                riso_one_pe_in_prog = false;
                clearONE_PEStarted();
            }
        }

        private bool analyze_in_prog;
        private int analyze_state = 0;
        private bool voltage_received;
        private bool current_received;
        private bool thd_c_received;
        private bool thd_v_received;
        private bool power_r_received;
        private bool power_a_received;
        private bool pf_received;

        private void MainsAnalyzeButton_Click(object sender, EventArgs e)
        {
            if (analyze_in_prog == false)
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__POWER, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__MACH, "", "");
                setAnalyzeStarted();
                analyze_state = 0;
                analyze_in_prog = true;
                //naslednje je zato, da se sploh zacne
                power_a_received = true;
                power_r_received = true;
                pf_received = true;
            }
            else
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__POWER, (int)serial_com.commandEnums.__STOP, (int)serial_com.stdEnums.__MACH, "", "");
                clearAnalyzeStarted();
                //task3.Abort();
            }
        }

        private void MainsAnalyzeTask()
        {
            while (analyze_in_prog)
            {
                Thread.Sleep(50);
                switch (analyze_state)
                {
                    case 0:
                        if (power_a_received && power_r_received && pf_received)
                        {
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.ANALYZE_COMMAND_NAMES[2], "", "");
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.ANALYZE_COMMAND_NAMES[4], "", "");
                            voltage_received = false;
                            current_received = false;
                            analyze_state++;
                        }
                        break;
                    case 1:
                        if (current_received && voltage_received)
                        {
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.ANALYZE_COMMAND_NAMES[6], "", "");
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.ANALYZE_COMMAND_NAMES[8], "", "");
                            thd_v_received = false;
                            thd_c_received = false;
                            analyze_state++;
                        }
                        break;
                    case 2:
                        if (thd_c_received && thd_v_received)
                        {
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.ANALYZE_COMMAND_NAMES[10], "", "");
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.ANALYZE_COMMAND_NAMES[12], "", "");
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.ANALYZE_COMMAND_NAMES[14], "", "");
                            power_a_received = false;
                            power_r_received = false;
                            pf_received = false;
                            analyze_state = 0;
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        private void AnalyzeResults(object sender, string cmd, string value1, string value2, string value3, string value4)
        {
            if (String.Equals(cmd, serial_com.ANALYZE_COMMAND_NAMES[3])) //ANALYZE VOLTAGE
            {
                Invoke(new MethodInvoker(delegate() { UL1textBox.Text = value1; }));
                Invoke(new MethodInvoker(delegate() { UL2textBox.Text = value2; }));
                Invoke(new MethodInvoker(delegate() { UL3textBox.Text = value3; }));
                Invoke(new MethodInvoker(delegate() { UNPEtextBox.Text = value4; }));
                voltage_received = true;
            }
            else if (String.Equals(cmd, serial_com.ANALYZE_COMMAND_NAMES[5])) //ANALYZE CURRENT
            {
                Invoke(new MethodInvoker(delegate() { IL1textBox.Text = value1; }));
                Invoke(new MethodInvoker(delegate() { IL2textBox.Text = value2; }));
                Invoke(new MethodInvoker(delegate() { IL3textBox.Text = value3; }));
                current_received = true;
            }
            else if (String.Equals(cmd, serial_com.ANALYZE_COMMAND_NAMES[7])) //ANALYZE THD C
            {
                Invoke(new MethodInvoker(delegate() { THD_IL1textBox.Text = value1; }));
                Invoke(new MethodInvoker(delegate() { THD_IL2textBox.Text = value2; }));
                Invoke(new MethodInvoker(delegate() { THD_IL3textBox.Text = value3; }));
                thd_c_received = true;
            }
            else if (String.Equals(cmd, serial_com.ANALYZE_COMMAND_NAMES[9])) //ANALYZE THD V
            {
                Invoke(new MethodInvoker(delegate() { THD_ULN1textBox.Text = value1; }));
                Invoke(new MethodInvoker(delegate() { THD_ULN2textBox.Text = value2; }));
                Invoke(new MethodInvoker(delegate() { THD_ULN3textBox.Text = value3; }));
                thd_v_received = true;
            }
            else if (String.Equals(cmd, serial_com.ANALYZE_COMMAND_NAMES[11])) //ANALYZE POWER R
            {
                Invoke(new MethodInvoker(delegate() { P1textBox.Text = value1; }));
                Invoke(new MethodInvoker(delegate() { P2textBox.Text = value2; }));
                Invoke(new MethodInvoker(delegate() { P3textBox.Text = value3; }));
                Invoke(new MethodInvoker(delegate() { P3PtextBox.Text = value4; }));
                power_r_received = true;
            }
            else if (String.Equals(cmd, serial_com.ANALYZE_COMMAND_NAMES[13])) //ANALYZE POWER A
            {
                Invoke(new MethodInvoker(delegate() { S1textBox.Text = value1; }));
                Invoke(new MethodInvoker(delegate() { S2textBox.Text = value2; }));
                Invoke(new MethodInvoker(delegate() { S3textBox.Text = value3; }));
                Invoke(new MethodInvoker(delegate() { S3PtextBox.Text = value4; }));
                power_a_received = true;
            }
            else if (String.Equals(cmd, serial_com.ANALYZE_COMMAND_NAMES[15])) //ANALYZE PF
            {
                Invoke(new MethodInvoker(delegate() { PF1textBox.Text = value1; }));
                Invoke(new MethodInvoker(delegate() { PF2textBox.Text = value2; }));
                Invoke(new MethodInvoker(delegate() { PF3textBox.Text = value3; }));
                Invoke(new MethodInvoker(delegate() { PF3PtextBox.Text = value4; }));
                pf_received = true;
            }

        }

        private void reset_analyze_textbox()
        {
            UL1textBox.AppendText("0.0");
            UL2textBox.AppendText("0.0");
            UL3textBox.AppendText("0.0");
            IL1textBox.AppendText("0.0");
            IL2textBox.AppendText("0.0");
            IL3textBox.AppendText("0.0");
            UNPEtextBox.AppendText("0.0");
            P1textBox.AppendText("0.0");
            P2textBox.AppendText("0.0");
            P3PtextBox.AppendText("0.0");
            P3textBox.AppendText("0.0");
            S1textBox.AppendText("0.0");
            S2textBox.AppendText("0.0");
            S3textBox.AppendText("0.0");
            S3PtextBox.AppendText("0.0");
            THD_IL1textBox.AppendText("0.0");
            THD_IL2textBox.AppendText("0.0");
            THD_IL3textBox.AppendText("0.0");
            THD_ULN1textBox.AppendText("0.0");
            THD_ULN2textBox.AppendText("0.0");
            THD_ULN3textBox.AppendText("0.0");
            PF1textBox.AppendText("0.0");
            PF2textBox.AppendText("0.0");
            PF3textBox.AppendText("0.0");
            PF3PtextBox.AppendText("0.0");
        }
        private string getCableTypeComboValue()
        {
            string krneki = null;
            this.Invoke(new MethodInvoker(delegate() { krneki = Cable_type_comboBox.Text; }));
            return krneki;
        }
    }
}
