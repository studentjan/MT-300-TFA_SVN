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
    public partial class Welding_form : Form
    {
        private serial_com Serial_object;
        private Form1 MainForm;
        private Thread task1;
        private Thread task2;
        private Thread task3;

        private int weld_ures_count=0;
        private bool weld_ures_in_prog = false;
        private bool weld_ures_started = false;
        private bool weld_ures_finished = false;

        private bool rpe_in_prog = false;
        private bool riso_all_pe_in_prog = false;
        private bool riso_one_pe_in_prog = false;
        private bool riso_mains_weld_in_prog = false;
        private bool riso_mains_acc_in_prog = false;
        private bool riso_weld_pe_in_prog = false;
        private bool unl_rms_in_prog = false;
        private bool unl_peak_in_prog = false;


        private string prev_msg = "";
        private string current_meas;
        private string delimiter = "|";
        public Welding_form(serial_com S_object, Form1 main_form)
        {
            InitializeComponent();
            Cable_type_comboBox.SelectedIndex = 0;
            RisoLimitTextbox.AppendText("1.0");
            return_iso_ok_textbox.AppendText("10.0");
            return_iso_nok_textbox.AppendText("0.5");
            Serial_object = S_object;
            Serial_object.weldReturnEventHandler += OnMachReturnResult;
            Serial_object.analyzeReturnEventHandler += AnalyzeResults;
            iso_state_combobox.SelectedIndex = 0;
            MainForm = main_form;
            //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
            //                    Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
            //                    serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[1], "", "");
            reset_analyze_textbox();
        }
        private void Welding_form_FormClosing(object sender, FormClosingEventArgs e)
        {
            MainForm.Enable_buttons();
            //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
            //                    Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
            //                    serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[2], "", "");
        }

        private void setURESstarted()
        {
            //this.Invoke(new MethodInvoker(delegate() { button1.Text = "URES_STOP"; }));
            weld_ures_started = true;
            rpe_button.Enabled = false;
            riso_button.Enabled = false;
        }
        private void clearURESstarted()
        {
            //this.Invoke(new MethodInvoker(delegate() { button1.Text = "URES_START"; }));
            weld_ures_started = false;
            weld_ures_finished = false;
            weld_ures_in_prog = false;
            weld_ures_count = 0;
            rpe_button.Enabled = true;
            riso_button.Enabled = true;
        }
        private void UREStask()
        {
            do
            {
                Thread.Sleep(100);
                switch (weld_ures_count)
                {
                    case 0:
                        if (weld_ures_started)
                        {
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[7], "", "");
                            weld_ures_count++;
                        }
                        break;
                    case 1:
                        if (weld_ures_finished)
                        {
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[7], "", "");
                            weld_ures_count++;
                        }
                        break;
                    case 2:
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[5], "", "");
                        weld_ures_count++;
                        //clearURESstarted(); sele ko dobimo stopped
                        break;
                    default:
                        break;
                }
            } while (weld_ures_in_prog);
        }

        private void stop_weld()
        {
            clearURESstarted();
        }

        public void OnMachReturnResult(object sender, string returned_string, string command, string leftover)
        {
            if (String.Equals(command, serial_com.WELD_COMMAND_NAMES[3])) //URES STARTED
            {
                setURESstarted();
            }
            else if (String.Equals(command, serial_com.WELD_COMMAND_NAMES[6])) //STOP
            {
                stop_weld();
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
                    weld_return_RISO_result(leftover);
                    if(riso_all_pe_in_prog)
                        clearALL_PEStarted();
                }
                else if (String.Equals(returned_string, serial_com.COMMAND_COMMUNICATON_NAMES[2]))
                {
                    weld_return_RISO_result(leftover);
                    if(riso_one_pe_in_prog)
                        clearONE_PEStarted();
                }
            }
            

        }

        private void AnalyzeResults(object sender, string cmd, string value1, string value2, string value3, string value4)
        {
            if (String.Equals(cmd, serial_com.ANALYZE_COMMAND_NAMES[3])) //ANALYZE VOLTAGE
            {
                Invoke(new MethodInvoker(delegate() { UL1textBox.Text = value1; }));
                Invoke(new MethodInvoker(delegate() { UL2textBox.Text = value2;}));
                Invoke(new MethodInvoker(delegate() { UL3textBox.Text = value3;}));
                Invoke(new MethodInvoker(delegate() { UNPEtextBox.Text = value4;}));
                voltage_received = true;
            }
            else if (String.Equals(cmd, serial_com.ANALYZE_COMMAND_NAMES[5])) //ANALYZE CURRENT
            {
                Invoke(new MethodInvoker(delegate() { IL1textBox.Text = value1;}));
                Invoke(new MethodInvoker(delegate() { IL2textBox.Text = value2;}));
                Invoke(new MethodInvoker(delegate() { IL3textBox.Text = value3;}));
                current_received = true;
            }
            else if (String.Equals(cmd, serial_com.ANALYZE_COMMAND_NAMES[7])) //ANALYZE THD C
            {
                Invoke(new MethodInvoker(delegate() { THD_IL1textBox.Text = value1;}));
                Invoke(new MethodInvoker(delegate() { THD_IL2textBox.Text = value2;}));
                Invoke(new MethodInvoker(delegate() { THD_IL3textBox.Text = value3;}));
                thd_c_received = true;
            }
            else if (String.Equals(cmd, serial_com.ANALYZE_COMMAND_NAMES[9])) //ANALYZE THD V
            {
                Invoke(new MethodInvoker(delegate() { THD_ULN1textBox.Text = value1;}));
                Invoke(new MethodInvoker(delegate() { THD_ULN2textBox.Text = value2;}));
                Invoke(new MethodInvoker(delegate() { THD_ULN3textBox.Text = value3; }));
                thd_v_received = true;
            }
            else if (String.Equals(cmd, serial_com.ANALYZE_COMMAND_NAMES[11])) //ANALYZE POWER R
            {
                Invoke(new MethodInvoker(delegate() { P1textBox.Text = value1;}));
                Invoke(new MethodInvoker(delegate() { P2textBox.Text = value2;}));
                Invoke(new MethodInvoker(delegate() { P3textBox.Text = value3;}));
                Invoke(new MethodInvoker(delegate() { P3PtextBox.Text = value4;}));
                power_r_received = true;
            }
            else if (String.Equals(cmd, serial_com.ANALYZE_COMMAND_NAMES[13])) //ANALYZE POWER A
            {
                Invoke(new MethodInvoker(delegate() { S1textBox.Text = value1;}));
                Invoke(new MethodInvoker(delegate() { S2textBox.Text = value2;}));
                Invoke(new MethodInvoker(delegate() { S3textBox.Text = value3;}));
                Invoke(new MethodInvoker(delegate() { S3PtextBox.Text = value4; }));
                power_a_received = true;
            }
            else if (String.Equals(cmd, serial_com.ANALYZE_COMMAND_NAMES[15])) //ANALYZE PF
            {
                Invoke(new MethodInvoker(delegate() { PF1textBox.Text = value1;}));
                Invoke(new MethodInvoker(delegate() { PF2textBox.Text = value2;}));
                Invoke(new MethodInvoker(delegate() { PF3textBox.Text = value3;}));
                Invoke(new MethodInvoker(delegate() { PF3PtextBox.Text = value4; }));
                pf_received = true;
            }
            
        }
        public void weld_return_RISO_result(string meas_con)
        {
            if (riso_all_pe_in_prog || riso_one_pe_in_prog || riso_mains_weld_in_prog || riso_mains_acc_in_prog || riso_weld_pe_in_prog)
            {
                prev_msg = meas_con;
                //task1_ex = true;
                task2 = new Thread(weld_get_RISO_task);
                task2.Start();
                current_meas = meas_con;
            }
        }

        private void weld_get_RISO_task()
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
            }
            else if (riso_one_pe_in_prog)
            {
                switch (getCableInsulationComboValue())
                {
                    case "L1 - PE":
                        if (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1]))
                            temp_str =  _passTempString;
                        else
                            temp_str =  _failTempString;
                        break;
                    case "L3 - PE":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[2])))
                            temp_str =  _failTempString;
                        else
                            temp_str =  _passTempString;
                        break;
                    case "L1,N -PE":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[2])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[3])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[6])))
                            temp_str =  _failTempString;
                        else
                            temp_str =  _passTempString;
                        break;
                    case "N - PE":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[2])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[3])))
                            temp_str =  _failTempString;
                        else
                            temp_str =  _passTempString;
                        break;
                    default:
                        temp_str =  _passTempString;
                        break;
                }
            }
            else
            {
                temp_str = "";
            }
            Serial_object.returnRisoRes(temp_str);
            //Serial_object.Send_protocol_message(
            //                    Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
            //                    Settings1.Default._ID_TFA, serial_com.COMMAND_TYPE_NAMES[8],
            //                    serial_com.WELD_COMMAND_NAMES[24], temp_str, "");
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
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = false; }));
        }
        private void clearRPEStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Text = "RPE START"; }));
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = true; }));
            rpe_in_prog = false;
        }

        private void setALL_PEStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { riso_button.Text = "STOP ALL-PE"; }));
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = false; }));
        }
        private void clearALL_PEStarted()
        {
            if (riso_all_pe_in_prog)
            {
                this.Invoke(new MethodInvoker(delegate() { riso_button.Text = "START ALL-PE"; }));
                this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Enabled = true; }));
                this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = true; }));
                this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = true; }));
                this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Enabled = true; }));
                this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Enabled = true; }));
                this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Enabled = true; }));
                this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Enabled = true; }));
                this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = true; }));
            }
            riso_all_pe_in_prog = false;
        }

        private void setONE_PEStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Text = "STOP ONE-PE"; }));
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = false; }));
        }
        private void clearONE_PEStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Text = "START ONE-PE"; }));
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = true; }));
            riso_one_pe_in_prog = false;
        }
        private void setMAINS_WELDStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Text = "STOP MAINS-WELD"; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = false; }));
        }
        private void clearMAINS_WELDStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Text = "START MAINS-WELD"; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = true; }));
            riso_mains_weld_in_prog = false;
        }
        private void setMAINS_ACCStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Text = "STOP MAINS-AC"; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = false; }));
        }
        private void clearMAINS_ACCStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Text = "START MAINS-AC"; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = true; }));
            riso_mains_acc_in_prog = false;
        }
        private void setWELD_PEStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Text = "STOP WELD-PE"; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = false; }));
        }
        private void clearWELD_PEStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Text = "START WELD-PE"; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = true; }));

            riso_weld_pe_in_prog = false;
        }
        private void setUNL_PeakStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Text = "STOP UNL PEAK"; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = false; }));
        }
        private void clearUNL_PeakStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Text = "START UNL PEAK"; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = true; }));
            unl_peak_in_prog = false;
        }
        private void setUNL_RMSStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Text = "STOP UNL RMS"; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = false; }));
        }
        private void clearUNL_RMSStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Text = "START UNL RMS"; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Enabled = true; }));
            unl_rms_in_prog = false;
        }
        private void setAnalyzeStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Text = "STOP MAINS ANALYZE"; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Enabled = false; }));
        }
        private void clearAnalyzeStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { MainsAnalyzeButton.Text = "START MAINS ANALYZE"; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { mains_weld_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { mains_acc_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_PEAK_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { weld_pe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { UNL_RMS_button.Enabled = true; }));
            analyze_in_prog = false;
        }
        private void rpe_button_Click(object sender, EventArgs e)
        {
            if (rpe_in_prog == false)
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__M_RPE, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__WELD, "", "");
                //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                //                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                //                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[11], "" ,"");
                rpe_in_prog = true;
            }
            else
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__M_RPE, (int)serial_com.commandEnums.__STOP, (int)serial_com.stdEnums.__WELD, "", "");
                //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                //                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                //                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[12], "", "");
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
                Serial_object.transmittComand((int)serial_com.funcEnums.__ALL_PE, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__WELD, temp_str2, "");
                //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                //                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                //                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[15], "", "");
                riso_all_pe_in_prog = true;
                setALL_PEStarted();
            }
            else
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__ALL_PE, (int)serial_com.commandEnums.__STOP, (int)serial_com.stdEnums.__WELD, "", "");
                //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                //                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                //                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[6], "", "");
                clearALL_PEStarted();
                riso_all_pe_in_prog = false;
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
                Serial_object.transmittComand((int)serial_com.funcEnums.__ONE_PE, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__WELD, temp_str2, "");
                riso_one_pe_in_prog = true;
                setONE_PEStarted();
            }
            else
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__ONE_PE, (int)serial_com.commandEnums.__STOP, (int)serial_com.stdEnums.__WELD, "", "");
                riso_one_pe_in_prog = false;
                clearONE_PEStarted();
            }
        }

        private void mains_weld_button_Click(object sender, EventArgs e)
        {
            if (riso_mains_weld_in_prog == false)
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__MAINS_WELD, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__WELD, "", "");
                //string temp_str = riso_cont_checkbox.Checked ? serial_com.WELD_CODE_NAMES[1] : serial_com.WELD_CODE_NAMES[0];
                //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                //                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                //                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[25], temp_str, "");
                riso_mains_weld_in_prog = true;
                setMAINS_WELDStarted();
            }
            else
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__MAINS_WELD, (int)serial_com.commandEnums.__STOP, (int)serial_com.stdEnums.__WELD, "", "");
                //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                //                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                //                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[6], "", "");
                //clearURESstarted();
                riso_mains_weld_in_prog = false;
                clearMAINS_WELDStarted();
            }
        }

        private void weld_pe_button_Click(object sender, EventArgs e)
        {
            
            if (riso_weld_pe_in_prog == false)
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__WELD_PE, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__WELD, "", "");
                //string temp_str = riso_cont_checkbox.Checked ? serial_com.WELD_CODE_NAMES[1] : serial_com.WELD_CODE_NAMES[0];
                
                //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                //                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                //                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[27], temp_str, "");
                riso_weld_pe_in_prog = true;
                setWELD_PEStarted();
            }
            else
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__WELD_PE, (int)serial_com.commandEnums.__STOP, (int)serial_com.stdEnums.__WELD, "", "");
                //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                //                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                //                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[6], "", "");
                //clearURESstarted();
                riso_weld_pe_in_prog = false;
                clearWELD_PEStarted();
            }
        }

        private void mains_acc_button_Click(object sender, EventArgs e)
        {
            if (riso_mains_acc_in_prog == false)
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__MAINS_CLASS2, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__WELD, "", "");
                //string temp_str = riso_cont_checkbox.Checked ? serial_com.WELD_CODE_NAMES[1] : serial_com.WELD_CODE_NAMES[0];
                //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                //                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                //                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[26], temp_str, "");
                riso_mains_acc_in_prog = true;
                setMAINS_ACCStarted();
            }
            else
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__MAINS_CLASS2, (int)serial_com.commandEnums.__STOP, (int)serial_com.stdEnums.__WELD, "", "");
                //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                //                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                //                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[6], "", "");
                //clearURESstarted();
                riso_mains_acc_in_prog = false;
                clearMAINS_ACCStarted();
            }
        }

        private void UNL_RMS_button_Click(object sender, EventArgs e)
        {
            if (unl_rms_in_prog == false)
            {
                //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                //                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                //                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[31], "", "");
                Serial_object.transmittComand((int)serial_com.funcEnums.__UNL_RMS, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__WELD, "", "");
                unl_rms_in_prog = true;
                setUNL_RMSStarted();
            }
            else
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__UNL_RMS, (int)serial_com.commandEnums.__STOP, (int)serial_com.stdEnums.__WELD, "", "");
                //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                //                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                //                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[32], "", "");
                clearURESstarted();
                unl_rms_in_prog = false;
                clearUNL_RMSStarted();
            }
        }

        private void UNL_PEAK_button_Click(object sender, EventArgs e)
        {
            if (unl_peak_in_prog == false)
            {
                Serial_object.transmittComand((int)serial_com.funcEnums.__UNL_PEAK, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__WELD, "", "");
                //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                //                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                //                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[33], "", "");
                unl_peak_in_prog = true;
                setUNL_PeakStarted();
            }
            else
            {
                //Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                //                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                //                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[34], "", "");
                Serial_object.transmittComand((int)serial_com.funcEnums.__UNL_PEAK, (int)serial_com.commandEnums.__STOP, (int)serial_com.stdEnums.__WELD, "", "");
                clearUNL_PeakStarted();
                unl_peak_in_prog = false;
            }
        }
        private bool analyze_in_prog;
        private int analyze_state=0;
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
                Serial_object.transmittComand((int)serial_com.funcEnums.__POWER, (int)serial_com.commandEnums.__START, (int)serial_com.stdEnums.__WELD, "", "");
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
                Serial_object.transmittComand((int)serial_com.funcEnums.__POWER, (int)serial_com.commandEnums.__STOP, (int)serial_com.stdEnums.__WELD, "", "");
                clearAnalyzeStarted();
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
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.ANALYZE_COMMAND_NAMES[2], "", "");
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.ANALYZE_COMMAND_NAMES[4], "", "");
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
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.ANALYZE_COMMAND_NAMES[6], "", "");
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.ANALYZE_COMMAND_NAMES[8], "", "");
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
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.ANALYZE_COMMAND_NAMES[10], "", "");
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.ANALYZE_COMMAND_NAMES[12], "", "");
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.ANALYZE_COMMAND_NAMES[14], "", "");
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
