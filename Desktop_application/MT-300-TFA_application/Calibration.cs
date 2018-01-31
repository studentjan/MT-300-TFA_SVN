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
    public partial class Calibration : Form
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

        private enum voltmeter
        {
            ULN1,
            ULN2,
            ULN3,
            IL1,
            IL2,
            IL3,
            UL1PE,
            UNPE
        }

        private string prev_msg = "";
        private string current_meas;
        private string delimiter = "|";
        public Calibration(serial_com S_object, Form1 main_form)
        {
            InitializeComponent();
            Serial_object = S_object;
            reset_textbox();
            Serial_object.calibReturnEventHandler += OnCalibReturnResult;
            MainForm = main_form;
        }


        public void OnCalibReturnResult(object sender, string command, string value1, string value2)
        {
            if (String.Equals(command, serial_com.CALIB_COMMAND_NAMES[1])) //VOLTAGE
            {
                if (String.Equals(value1, serial_com.CALIB_ADD_NAMES[0])) // ULN1
                {
                    if (ULN1_low_requested)
                    {
                        Invoke(new MethodInvoker(delegate() { UL1lowRetTextBox.Text = value2; }));
                        ULN1_low_requested = false;
                    }
                    else if (ULN1_high_requested)
                    {
                        Invoke(new MethodInvoker(delegate() { UL1highRetTextBox.Text = value2; }));
                        ULN1_high_requested = false;
                    }
                    EnableAllButtons();
                }
                else if (String.Equals(value1, serial_com.CALIB_ADD_NAMES[1])) // ULN2
                {
                    if (ULN2_low_requested)
                    {
                        Invoke(new MethodInvoker(delegate() { UL2lowRetTextBox.Text = value2; }));
                        ULN2_low_requested = false;
                    }
                    else if (ULN2_high_requested)
                    {
                        Invoke(new MethodInvoker(delegate() { UL2highRetTextBox.Text = value2; }));
                        ULN2_high_requested = false;
                    }
                    EnableAllButtons();
                }
                else if (String.Equals(value1, serial_com.CALIB_ADD_NAMES[2])) // ULN3
                {
                    if (ULN3_low_requested)
                    {
                        Invoke(new MethodInvoker(delegate() { UL3lowRetTextBox.Text = value2; }));
                        ULN3_low_requested = false;
                    }
                    else if (ULN3_high_requested)
                    {
                        Invoke(new MethodInvoker(delegate() { UL3highRetTextBox.Text = value2; }));
                        ULN3_high_requested = false;
                    }
                    EnableAllButtons();
                }
                else if (String.Equals(value1, serial_com.CALIB_ADD_NAMES[10])) // UL1PE
                {
                    if (UL1PE_low_requested)
                    {
                        Invoke(new MethodInvoker(delegate() { UL1PElowRetTextBox.Text = value2; }));
                        UL1PE_low_requested = false;
                    }
                    else if (UL1PE_high_requested)
                    {
                        Invoke(new MethodInvoker(delegate() { UL1PEhighRetTextBox.Text = value2; }));
                        UL1PE_high_requested = false;
                    }
                    EnableAllButtons();
                }
                else if (String.Equals(value1, serial_com.CALIB_ADD_NAMES[9])) // UNPE
                {
                    if (UNPE_low_requested)
                    {
                        Invoke(new MethodInvoker(delegate() { UNPElowRetTextBox.Text = value2; }));
                        UNPE_low_requested = false;
                    }
                    else if (UNPE_high_requested)
                    {
                        Invoke(new MethodInvoker(delegate() { UNPEhighRetTextBox.Text = value2; }));
                        UNPE_high_requested = false;
                    }
                    EnableAllButtons();
                }
                else
                {
                    ULN1_high_requested = false;
                    ULN2_high_requested = false;
                    ULN3_high_requested = false;
                    ULN1_low_requested = false;
                    ULN2_low_requested = false;
                    ULN3_low_requested = false;
                    UL1PE_high_requested = false;
                    UNPE_high_requested = false;
                    UL1PE_low_requested = false;
                    UNPE_low_requested = false;
                    EnableAllButtons();
                }
            }
            
            if (String.Equals(command, serial_com.CALIB_COMMAND_NAMES[5])) //CONSTANTS
            {
                setConstants(value1, value2);
            }


        }

        private void setConstants(string constName, string constValue)
        {
            if (String.Equals(constName, serial_com.CALIB_ADD_NAMES[3]))
            {
                Invoke(new MethodInvoker(delegate() { ULN1kTextbox.Text = constValue; }));
            }
            else if (String.Equals(constName, serial_com.CALIB_ADD_NAMES[4]))
            {
                Invoke(new MethodInvoker(delegate() { ULN1nTextbox.Text = constValue; }));
            }
            else if (String.Equals(constName, serial_com.CALIB_ADD_NAMES[5]))
            {
                Invoke(new MethodInvoker(delegate() { ULN2kTextbox.Text = constValue; }));
            }
            else if (String.Equals(constName, serial_com.CALIB_ADD_NAMES[6]))
            {
                Invoke(new MethodInvoker(delegate() { ULN2nTextbox.Text = constValue; }));
            }
            else if (String.Equals(constName, serial_com.CALIB_ADD_NAMES[7]))
            {
                Invoke(new MethodInvoker(delegate() { ULN3kTextbox.Text = constValue; }));
            }
            else if (String.Equals(constName, serial_com.CALIB_ADD_NAMES[8]))
            {
                Invoke(new MethodInvoker(delegate() { ULN3nTextbox.Text = constValue; }));
            }
            else if (String.Equals(constName, serial_com.CALIB_ADD_NAMES[11]))
            {
                Invoke(new MethodInvoker(delegate() { UNPEkTextbox.Text = constValue; }));
            }
            else if (String.Equals(constName, serial_com.CALIB_ADD_NAMES[12]))
            {
                Invoke(new MethodInvoker(delegate() { UL1PEkTextbox.Text = constValue; }));
            }
            else if (String.Equals(constName, serial_com.CALIB_ADD_NAMES[13]))
            {
                Invoke(new MethodInvoker(delegate() { UNPEnTextbox.Text = constValue; }));
            }
            else if (String.Equals(constName, serial_com.CALIB_ADD_NAMES[14]))
            {
                Invoke(new MethodInvoker(delegate() { UL1PEnTextbox.Text = constValue; }));
            }
        }

        private void computeConstants(int vmeter)
        {
            if (vmeter == (int) voltmeter.ULN1)
            {
                float y1 = float.Parse(UL1lowRetTextBox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float y2 = float.Parse(UL1highRetTextBox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float x1 = float.Parse(UL1lowTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float x2 = float.Parse(UL1highTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float k2prev = float.Parse(ULN1kTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float n2prev = float.Parse(ULN1nTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float k2 = ((y2 - y1)/(x2 - x1));
                float n2 = y1 - (k2*x1);
                float n2new = -n2;
                float k2new = k2prev/k2;
                Invoke(new MethodInvoker(delegate() { ULN1nTextbox.Text = n2new.ToString(); }));
                Invoke(new MethodInvoker(delegate() { ULN1kTextbox.Text = k2new.ToString(); }));
            }
            else if (vmeter == (int)voltmeter.ULN2)
            {
                float y1 = float.Parse(UL2lowRetTextBox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float y2 = float.Parse(UL2highRetTextBox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float x1 = float.Parse(UL2lowTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float x2 = float.Parse(UL2highTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float k2prev = float.Parse(ULN2kTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float n2prev = float.Parse(ULN2nTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float k2 = ((y2 - y1) / (x2 - x1));
                float n2 = y1 - (k2 * x1);
                float n2new = -n2;
                float k2new = k2prev / k2;
                Invoke(new MethodInvoker(delegate() { ULN2nTextbox.Text = n2new.ToString(); }));
                Invoke(new MethodInvoker(delegate() { ULN2kTextbox.Text = k2new.ToString(); }));
            }
            else if (vmeter == (int)voltmeter.ULN3)
            {
                float y1 = float.Parse(UL3lowRetTextBox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float y2 = float.Parse(UL3highRetTextBox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float x1 = float.Parse(UL3lowTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float x2 = float.Parse(UL3highTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float k2prev = float.Parse(ULN3kTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float n2prev = float.Parse(ULN3nTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float k2 = ((y2 - y1) / (x2 - x1));
                float n2 = y1 - (k2 * x1);
                float n2new = -n2;
                float k2new = k2prev / k2;
                Invoke(new MethodInvoker(delegate() { ULN3nTextbox.Text = n2new.ToString(); }));
                Invoke(new MethodInvoker(delegate() { ULN3kTextbox.Text = k2new.ToString(); }));
            }
            else if (vmeter == (int)voltmeter.UL1PE)
            {
                float y1 = float.Parse(UL1PElowRetTextBox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float y2 = float.Parse(UL1PEhighRetTextBox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float x1 = float.Parse(UL1PElowTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float x2 = float.Parse(UL1PEhighTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float k2prev = float.Parse(UL1PEkTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float n2prev = float.Parse(UL1PEnTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float k2 = ((y2 - y1) / (x2 - x1));
                float n2 = y1 - (k2 * x1);
                float n2new = -n2;
                float k2new = k2prev / k2;
                Invoke(new MethodInvoker(delegate() { UL1PEnTextbox.Text = n2new.ToString(); }));
                Invoke(new MethodInvoker(delegate() { UL1PEkTextbox.Text = k2new.ToString(); }));
            }
            else if (vmeter == (int)voltmeter.UNPE)
            {
                float y1 = float.Parse(UNPElowRetTextBox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float y2 = float.Parse(UNPEhighRetTextBox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float x1 = float.Parse(UNPElowTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float x2 = float.Parse(UNPEhighTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float k2prev = float.Parse(UNPEkTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float n2prev = float.Parse(UNPEnTextbox.Text, System.Globalization.CultureInfo.InvariantCulture);
                float k2 = ((y2 - y1) / (x2 - x1));
                float n2 = y1 - (k2 * x1);
                float n2new = -n2;
                float k2new = k2prev / k2;
                Invoke(new MethodInvoker(delegate() { UNPEnTextbox.Text = n2new.ToString(); }));
                Invoke(new MethodInvoker(delegate() { UNPEkTextbox.Text = k2new.ToString(); }));
            }
        }

        private void transmittConstants(int vmeter)
        {
            string temp_str;
            if (vmeter == (int) voltmeter.ULN1)
            {
                temp_str = string.Format("ULN1_K|{0},ULN1_N|{1}", ULN1kTextbox.Text, ULN1nTextbox.Text);
            }
            else if (vmeter == (int)voltmeter.ULN2)
            {
                temp_str = string.Format("ULN2_K|{0},ULN2_N|{1}", ULN2kTextbox.Text, ULN2nTextbox.Text);
            }
            else if (vmeter == (int) voltmeter.ULN3)
            {
                temp_str = string.Format("ULN3_K|{0},ULN3_N|{1}", ULN3kTextbox.Text, ULN3nTextbox.Text);
            }
            else if (vmeter == (int)voltmeter.UL1PE)
            {
                temp_str = string.Format("UL1PE_K|{0},UL1PE_N|{1}", UL1PEkTextbox.Text, UL1PEnTextbox.Text);
            }
            else if (vmeter == (int)voltmeter.UNPE)
            {
                temp_str = string.Format("UNPE_K|{0},UNPE_N|{1}", UNPEkTextbox.Text, UNPEnTextbox.Text);
            }
            else
            {
                temp_str = "";
            }
            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                            Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                            serial_com.COMMAND_TYPE_NAMES[9], serial_com.CALIB_COMMAND_NAMES[5], temp_str, "");
        }
        private void reset_textbox()
        {
            UL1lowTextbox.AppendText("0.0");
            UL1lowRetTextBox.AppendText("0.0");
            UL1highRetTextBox.AppendText("0.0");
            UL1highTextbox.AppendText("0.0");
            UL2highRetTextBox.AppendText("0.0");
            UL2highTextbox.AppendText("0.0");
            UL2lowRetTextBox.AppendText("0.0");
            UL2lowTextbox.AppendText("0.0");
            UL3highRetTextBox.AppendText("0.0");
            UL3highTextbox.AppendText("0.0");
            UL3lowRetTextBox.AppendText("0.0");
            UL3lowTextbox.AppendText("0.0");
            ULN1kTextbox.AppendText("1.0");
            ULN1nTextbox.AppendText("0.0");
            ULN2nTextbox.AppendText("0.0");
            ULN2kTextbox.AppendText("1.0");
            ULN3kTextbox.AppendText("1.0");
            ULN3nTextbox.AppendText("0.0");
            UL1PEnTextbox.AppendText("0.0");
            UL1PEkTextbox.AppendText("1.0");
            UNPEkTextbox.AppendText("1.0");
            UNPEnTextbox.AppendText("0.0");
        }

        private bool ULN1_low_requested;
        private bool ULN2_low_requested;
        private bool ULN3_low_requested;
        private bool ULN3_high_requested;
        private bool ULN2_high_requested;
        private bool ULN1_high_requested;
        private bool UL1PE_low_requested;
        private bool UNPE_low_requested;
        private bool UL1PE_high_requested;
        private bool UNPE_high_requested;
        private void ULN1lowButton_Click(object sender, EventArgs e)
        {
            DisableAllButtons();
            ULN1_low_requested = true;
            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[9], serial_com.CALIB_COMMAND_NAMES[0], "", "");
        }

        private void ULN1highButton_Click(object sender, EventArgs e)
        {
            DisableAllButtons();
            ULN1_high_requested = true;
            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[9], serial_com.CALIB_COMMAND_NAMES[0], "", "");
        }

        private void ULN2lowButton_Click(object sender, EventArgs e)
        {
            DisableAllButtons();
            ULN2_low_requested = true;
            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[9], serial_com.CALIB_COMMAND_NAMES[2], "", "");
        }

        private void ULN2highButton_Click(object sender, EventArgs e)
        {
            DisableAllButtons();
            ULN2_high_requested = true;
            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[9], serial_com.CALIB_COMMAND_NAMES[2], "", "");
        }

        private void ULN3lowButton_Click(object sender, EventArgs e)
        {
            DisableAllButtons();
            ULN3_low_requested = true;
            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[9], serial_com.CALIB_COMMAND_NAMES[3], "", "");
        }

        private void ULN3highButton_Click(object sender, EventArgs e)
        {
            DisableAllButtons();
            ULN3_high_requested = true;
            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[9], serial_com.CALIB_COMMAND_NAMES[3], "", "");
        }

        private void ULN1computeButton_Click(object sender, EventArgs e)
        {
            computeConstants((int)voltmeter.ULN1);
        }

        private void ULN2computeButton_Click(object sender, EventArgs e)
        {
            computeConstants((int)voltmeter.ULN2);
        }

        private void ULN3computeButton_Click(object sender, EventArgs e)
        {
            computeConstants((int)voltmeter.ULN3);
        }

        private void ULN1saveButton_Click(object sender, EventArgs e)
        {
            transmittConstants((int) voltmeter.ULN1);
        }

        private void ULN2saveButton_Click(object sender, EventArgs e)
        {
            transmittConstants((int)voltmeter.ULN2);
        }

        private void ULN3saveButton_Click(object sender, EventArgs e)
        {
            transmittConstants((int)voltmeter.ULN3);
        }

 
        private void EnableAllButtons()
        {
            Invoke(new MethodInvoker(delegate() { ULN1computeButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { ULN2computeButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { ULN3computeButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { ULN1saveButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { ULN2saveButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { ULN3saveButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { ULN1highButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { ULN2highButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { ULN3highButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { ULN1lowButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { ULN2lowButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { ULN3lowButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { UL1PElowButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { UNPElowButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { UL1PEhighButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { UNPEhighButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { UNPEcomputeButton.Enabled = true; }));
            Invoke(new MethodInvoker(delegate() { UL1PEcomputeButton.Enabled = true; }));
        }

        private void DisableAllButtons()
        {
            Invoke(new MethodInvoker(delegate() { ULN1computeButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { ULN2computeButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { ULN3computeButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { ULN1saveButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { ULN2saveButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { ULN3saveButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { ULN1highButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { ULN2highButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { ULN3highButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { ULN1lowButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { ULN2lowButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { ULN3lowButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { UL1PElowButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { UNPElowButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { UL1PEhighButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { UNPEhighButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { UNPEcomputeButton.Enabled = false; }));
            Invoke(new MethodInvoker(delegate() { UL1PEcomputeButton.Enabled = false; }));
        }

        private void Calibration_FormClosing(object sender, FormClosingEventArgs e)
        {
            MainForm.Enable_buttons();
        }

        private void getConstantsButton_Click(object sender, EventArgs e)
        {
            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[9], serial_com.CALIB_COMMAND_NAMES[4], "", "");
        }

        private void UL1PElowButton_Click(object sender, EventArgs e)
        {
            DisableAllButtons();
            UL1PE_low_requested = true;
            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[9], serial_com.CALIB_COMMAND_NAMES[6], "", "");
        }

        private void UL1PEhighButton_Click(object sender, EventArgs e)
        {
            DisableAllButtons();
            UL1PE_high_requested = true;
            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[9], serial_com.CALIB_COMMAND_NAMES[6], "", "");
        }

        private void UL1PEcomputeButton_Click(object sender, EventArgs e)
        {
            computeConstants((int)voltmeter.UL1PE);
        }

        private void UL1PEsaveButton_Click(object sender, EventArgs e)
        {
            transmittConstants((int)voltmeter.UL1PE);
        }

        private void UNPElowButton_Click(object sender, EventArgs e)
        {
            DisableAllButtons();
            UNPE_low_requested = true;
            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[9], serial_com.CALIB_COMMAND_NAMES[7], "", "");
        }

        private void UNPEhighButton_Click(object sender, EventArgs e)
        {
            DisableAllButtons();
            UNPE_high_requested = true;
            Serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1,
                                Settings1.Default._ID_MT, Settings1.Default._ID_TFA,
                                serial_com.COMMAND_TYPE_NAMES[9], serial_com.CALIB_COMMAND_NAMES[7], "", "");
        }

        private void UNPEcomputeButton_Click(object sender, EventArgs e)
        {
            computeConstants((int)voltmeter.UNPE);
        }

        private void UNPEsaveButton_Click(object sender, EventArgs e)
        {
            transmittConstants((int)voltmeter.UNPE);
        }



    }
}
