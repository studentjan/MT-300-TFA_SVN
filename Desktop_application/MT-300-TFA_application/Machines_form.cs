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
            Serial_object.machReturnEventHandler += OnMachReturnResult;
            iso_state_combobox.SelectedIndex = 0;
            Select_measurement_comboBox.SelectedIndex = 0;
            MainForm = main_form;
            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.MACH_COMMAND_NAMES[1], "", "");
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
            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.MACH_COMMAND_NAMES[2], "", "");
        }
        private string getMeasSelectedComboValue()
        {
            string krneki = null;
            this.Invoke(new MethodInvoker(delegate() { krneki = Select_measurement_comboBox.Text; }));
            return krneki;
        }
        private void Cable_type_comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (mach_ures_in_prog == false)
            {
                string temp_str;
                temp_str = getSetURES();
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.MACH_COMMAND_NAMES[0], temp_str, "");
                task1 = new Thread(UREStask);
                task1.Start();
                mach_ures_in_prog = true;
            }
            else
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.MACH_COMMAND_NAMES[6], "", "");
                clearURESstarted();
            }

        }

        private void setURESstarted()
        {
            //this.Invoke(new MethodInvoker(delegate() { button1.Text = "URES_STOP"; }));
            mach_ures_started = true;
            rpe_button.Enabled = false;
            riso_button.Enabled = false;
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
                Thread.Sleep(100);
                switch (mach_ures_count)
                {
                    case 0:
                        if (mach_ures_started)
                        {
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.MACH_COMMAND_NAMES[7], "", "");
                            mach_ures_count++;
                        }
                        break;
                    case 1:
                        if (mach_ures_finished)
                        {
                            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.MACH_COMMAND_NAMES[7], "", "");
                            mach_ures_count++;
                        }
                        break;
                    case 2:
                        Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.MACH_COMMAND_NAMES[5], "", "");
                        mach_ures_count++;
                        //clearURESstarted(); sele ko dobimo stopped
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

        public void OnMachReturnResult(object sender, string returned_string, string command)
        {
            if (String.Equals(command, serial_com.MACH_COMMAND_NAMES[3])) //URES STARTED
            {
                setURESstarted();
            }
            else if (String.Equals(command, serial_com.MACH_COMMAND_NAMES[6])) //STOP
            {
                stop_mach();
            }
            else if (String.Equals(command, serial_com.MACH_COMMAND_NAMES[8])) //URES OPENED
            {
                mach_ures_finished = true;
            }
            else if (String.Equals(command, serial_com.MACH_COMMAND_NAMES[4])) //URES STOPPED
            {
                clearURESstarted();
            }
            else if (String.Equals(command, serial_com.MACH_COMMAND_NAMES[13])) //RPE STARTED
            {
                setRPEStarted();
            }
            else if (String.Equals(command, serial_com.MACH_COMMAND_NAMES[14])) //RPE STOPPED
            {
                clearRPEStarted();
            }
            else if (String.Equals(command, serial_com.MACH_COMMAND_NAMES[17])) //ALL-PE RESULT
            {
                clearALL_PEStarted();
            }
            else if (String.Equals(command, serial_com.MACH_COMMAND_NAMES[18])) //ONE-PE RESULT
            {
                clearONE_PEStarted();
            }
            else if (String.Equals(command, serial_com.MACH_COMMAND_NAMES[19])) //start RISO START
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                               Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                               serial_com.COMMAND_TYPE_NAMES[7], serial_com.MACH_COMMAND_NAMES[20], "", "");
            }
            else if (String.Equals(command, serial_com.MACH_COMMAND_NAMES[21])) //start RISO STOP
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                               Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                               serial_com.COMMAND_TYPE_NAMES[7], serial_com.MACH_COMMAND_NAMES[22], "", "");
            }
            else if (String.Equals(command, serial_com.MACH_COMMAND_NAMES[23])) //get RISO resistance
            {
                mach_return_RISO_result(returned_string);
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
                                Settings1.Default._ID_TFA, serial_com.COMMAND_TYPE_NAMES[7],
                                serial_com.MACH_COMMAND_NAMES[24], temp_str, "");
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
        }
        private void clearRPEStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Text = "RPE START"; }));
            this.Invoke(new MethodInvoker(delegate() { button1.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = true; }));
            rpe_in_prog = false;
        }

        private void setALL_PEStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { riso_button.Text = "STOP ALL-PE"; }));
            this.Invoke(new MethodInvoker(delegate() { button1.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = false; }));
        }
        private void clearALL_PEStarted()
        {
            if (riso_all_pe_in_prog)
            {
                this.Invoke(new MethodInvoker(delegate() { riso_button.Text = "START ALL-PE"; }));
                this.Invoke(new MethodInvoker(delegate() { button1.Enabled = true; }));
                this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = true; }));
                this.Invoke(new MethodInvoker(delegate() { riso_button2.Enabled = true; }));
            }
            riso_all_pe_in_prog = false;
        }

        private void setONE_PEStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Text = "STOP ONE-PE"; }));
            this.Invoke(new MethodInvoker(delegate() { button1.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = false; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = false; }));
        }
        private void clearONE_PEStarted()
        {
            this.Invoke(new MethodInvoker(delegate() { riso_button2.Text = "START ONE-PE"; }));
            this.Invoke(new MethodInvoker(delegate() { button1.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { rpe_button.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { riso_button.Enabled = true; }));
            riso_one_pe_in_prog = false;
        }
        private void rpe_button_Click(object sender, EventArgs e)
        {
            if (rpe_in_prog == false)
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.MACH_COMMAND_NAMES[11], "" ,"");
                rpe_in_prog = true;
            }
            else
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.MACH_COMMAND_NAMES[12], "", "");
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
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.MACH_COMMAND_NAMES[15], "", "");
                riso_all_pe_in_prog = true;
                setALL_PEStarted();
            }
            else
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.MACH_COMMAND_NAMES[6], "", "");
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
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.MACH_COMMAND_NAMES[16], "", "");
                riso_one_pe_in_prog = true;
                setONE_PEStarted();
            }
            else
            {
                Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[7], serial_com.MACH_COMMAND_NAMES[6], "", "");
                clearURESstarted();
                riso_one_pe_in_prog = false;
                clearONE_PEStarted();
            }
        }

        

    }
}
