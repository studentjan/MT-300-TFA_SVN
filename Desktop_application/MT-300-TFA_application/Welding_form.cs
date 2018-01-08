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
            return_iso_ok_textbox.AppendText("10.0");
            return_iso_nok_textbox.AppendText("0.5");
            Serial_object = S_object;
            Serial_object.weldReturnEventHandler += OnMachReturnResult;
            iso_state_combobox.SelectedIndex = 0;
            MainForm = main_form;
            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[1], "", "");
        }
        private void Welding_form_FormClosing(object sender, FormClosingEventArgs e)
        {
            MainForm.Enable_buttons();
            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[2], "", "");
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

        public void OnMachReturnResult(object sender, string returned_string, string command)
        {
            if (String.Equals(command, serial_com.WELD_COMMAND_NAMES[3])) //URES STARTED
            {
                setURESstarted();
            }
            else if (String.Equals(command, serial_com.WELD_COMMAND_NAMES[6])) //STOP
            {
                stop_weld();
            }
            else if (String.Equals(command, serial_com.WELD_COMMAND_NAMES[13])) //RPE STARTED
            {
                setRPEStarted();
            }
            else if (String.Equals(command, serial_com.WELD_COMMAND_NAMES[14])) //RPE STOPPED
            {
                clearRPEStarted();
            }
            else if (String.Equals(command, serial_com.WELD_COMMAND_NAMES[17])) //ALL-PE RESULT
            {
                clearALL_PEStarted();
            }
            else if (String.Equals(command, serial_com.WELD_COMMAND_NAMES[18])) //ONE-PE RESULT
            {
                clearONE_PEStarted();
            }
            else if (String.Equals(command, serial_com.WELD_COMMAND_NAMES[19])) //start RISO START
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                               Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                               serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[20], "", "");
            }
            else if (String.Equals(command, serial_com.WELD_COMMAND_NAMES[21])) //start RISO STOP
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                               Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                               serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[22], "", "");
            }
            else if (String.Equals(command, serial_com.WELD_COMMAND_NAMES[23])) //get RISO resistance
            {
                weld_return_RISO_result(returned_string);
            }
            else if (String.Equals(command, serial_com.WELD_COMMAND_NAMES[28])) //Mains-weld result
            {
                clearMAINS_WELDStarted();
            }
            else if (String.Equals(command, serial_com.WELD_COMMAND_NAMES[29])) //Mains-class2 result
            {
                clearMAINS_ACCStarted();
            }
            else if (String.Equals(command, serial_com.WELD_COMMAND_NAMES[30])) //weld-PE result
            {
                clearWELD_PEStarted();
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
                        temp_str = serial_com.CORD_CODE_NAMES[29] + delimiter + _failTempString;
                        break;
                    case "L3 - PE":
                        temp_str = serial_com.CORD_CODE_NAMES[29] + delimiter + _failTempString;
                        break;
                    case "L1,N -PE":
                        temp_str = serial_com.CORD_CODE_NAMES[29] + delimiter + _failTempString;
                        break;
                    case "N - PE":
                        temp_str = serial_com.CORD_CODE_NAMES[29] + delimiter + _failTempString;
                        break;
                    default:
                        temp_str = serial_com.CORD_CODE_NAMES[29] + delimiter + _passTempString;
                        break;
                }
            }
            else if (riso_one_pe_in_prog)
            {
                switch (getCableInsulationComboValue())
                {
                    case "L1 - PE":
                        if (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1]))
                            temp_str = serial_com.CORD_CODE_NAMES[29] + delimiter + _passTempString;
                        else
                            temp_str = serial_com.CORD_CODE_NAMES[29] + delimiter + _failTempString;
                        break;
                    case "L3 - PE":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[2])))
                            temp_str = serial_com.CORD_CODE_NAMES[29] + delimiter + _failTempString;
                        else
                            temp_str = serial_com.CORD_CODE_NAMES[29] + delimiter + _passTempString;
                        break;
                    case "L1,N -PE":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[2])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[3])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[6])))
                            temp_str = serial_com.CORD_CODE_NAMES[29] + delimiter + _failTempString;
                        else
                            temp_str = serial_com.CORD_CODE_NAMES[29] + delimiter + _passTempString;
                        break;
                    case "N - PE":
                        if ((String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[1])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[2])) || (String.Equals(current_meas, serial_com.CORD_LEFTOVER_RISO_NAMES[3])))
                            temp_str = serial_com.CORD_CODE_NAMES[29] + delimiter + _failTempString;
                        else
                            temp_str = serial_com.CORD_CODE_NAMES[29] + delimiter + _passTempString;
                        break;
                    default:
                        temp_str = serial_com.CORD_CODE_NAMES[29] + delimiter + _passTempString;
                        break;
                }
            }
            else
            {
                temp_str = "";
            }
            Serial_object.Send_protocol_message(
                                Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT,
                                Settings1.Default._ID_TFA, serial_com.COMMAND_TYPE_NAMES[8],
                                serial_com.WELD_COMMAND_NAMES[24], temp_str, "");
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
            unl_rms_in_prog = false;
        }
        private void rpe_button_Click(object sender, EventArgs e)
        {
            if (rpe_in_prog == false)
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[11], "" ,"");
                rpe_in_prog = true;
            }
            else
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[12], "", "");
                clearURESstarted();
                rpe_in_prog = false;
            }

        }

        private void riso_button_Click(object sender, EventArgs e)
        {
            if (riso_all_pe_in_prog == false)
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[15], "", "");
                riso_all_pe_in_prog = true;
                setALL_PEStarted();
            }
            else
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[6], "", "");
                riso_all_pe_in_prog = false;
                clearALL_PEStarted();
            }
        }

        private void riso_button2_Click(object sender, EventArgs e)
        {
            if (riso_one_pe_in_prog == false)
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[16], "", "");
                riso_one_pe_in_prog = true;
                setONE_PEStarted();
            }
            else
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[6], "", "");
                clearURESstarted();
                riso_one_pe_in_prog = false;
                clearONE_PEStarted();
            }
        }

        private void mains_weld_button_Click(object sender, EventArgs e)
        {
            if (riso_mains_weld_in_prog == false)
            {
                string temp_str = riso_cont_checkbox.Checked ? serial_com.WELD_CODE_NAMES[1] : serial_com.WELD_CODE_NAMES[0];
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[25], temp_str, "");
                riso_mains_weld_in_prog = true;
                setMAINS_WELDStarted();
            }
            else
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[6], "", "");
                clearURESstarted();
                riso_mains_weld_in_prog = false;
                clearMAINS_WELDStarted();
            }
        }

        private void weld_pe_button_Click(object sender, EventArgs e)
        {
            
            if (riso_weld_pe_in_prog == false)
            {
                string temp_str = riso_cont_checkbox.Checked ? serial_com.WELD_CODE_NAMES[1] : serial_com.WELD_CODE_NAMES[0];
                
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[27], temp_str, "");
                riso_weld_pe_in_prog = true;
                setWELD_PEStarted();
            }
            else
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[6], "", "");
                clearURESstarted();
                riso_weld_pe_in_prog = false;
                clearWELD_PEStarted();
            }
        }

        private void mains_acc_button_Click(object sender, EventArgs e)
        {
            if (riso_mains_acc_in_prog == false)
            {
                string temp_str = riso_cont_checkbox.Checked ? serial_com.WELD_CODE_NAMES[1] : serial_com.WELD_CODE_NAMES[0];
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[26], temp_str, "");
                riso_mains_acc_in_prog = true;
                setMAINS_ACCStarted();
            }
            else
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[6], "", "");
                clearURESstarted();
                riso_mains_acc_in_prog = false;
                clearMAINS_ACCStarted();
            }
        }

        private void UNL_RMS_button_Click(object sender, EventArgs e)
        {
            if (unl_rms_in_prog == false)
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[31], "", "");
                unl_rms_in_prog = true;
                setUNL_RMSStarted();
            }
            else
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[32], "", "");
                clearURESstarted();
                unl_rms_in_prog = false;
                clearUNL_RMSStarted();
            }
        }

        private void UNL_PEAK_button_Click(object sender, EventArgs e)
        {
            if (unl_peak_in_prog == false)
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[33], "", "");
                unl_peak_in_prog = true;
                setUNL_PeakStarted();
            }
            else
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[8], serial_com.WELD_COMMAND_NAMES[34], "", "");
                clearURESstarted();
                unl_peak_in_prog = false;
                clearUNL_PeakStarted();
            }
        }

        

    }
}
