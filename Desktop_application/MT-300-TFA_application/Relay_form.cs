using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MT_300_TFA_application
{
    public partial class Relay_form : Form
    {
       
        private Form1 mainForm;
        //serial_com serial_com = new serial_com();
        private serial_com serial_object;

        public Relay_form(Form1 form1, serial_com krneki)
        {
            InitializeComponent();
            mainForm = form1;
            serial_object = krneki;
        }
        public Relay_form()
        {
            InitializeComponent();
            //Serial_class2 = new serial_com(this);
        }

        private void rel_1_38_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[1], "","");
        }

        private void rel_1_38_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[2], "","");
        }

        private void rel_2_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[3], "","");
        }

        private void rel_2_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[4], "","");
        }

        private void rel_3_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[5], "","");
        }

        private void rel_3_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[6], "","");
        }

        private void rel_4_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[7], "","");
        }

        private void rel_4_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[8], "","");
        }

        private void rel_5_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[9], "","");
        }

        private void rel_5_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[10], "","");
        }

        private void rel_6_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[11], "","");
        }

        private void rel_6_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[12], "","");
        }

        private void rel_7_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[13], "","");
        }

        private void rel_7_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[14], "","");
        }

        private void rel_8_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[15], "","");
        }

        private void rel_8_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[16], "","");
        }

        private void rel_9_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[17], "","");
        }

        private void rel_9_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[18], "","");
        }

        private void rel_10_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[19], "","");
        }

        private void rel_10_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[20], "","");
        }

        private void rel_11_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[21], "","");
        }

        private void rel_11_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[22], "","");
        }

        private void rel_12_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[23], "","");
        }

        private void rel_12_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[24], "","");
        }

        private void rel_13_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[25], "","");
        }

        private void rel_13_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[26], "","");
        }

        private void rel_14_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[27], "","");
        }

        private void rel_14_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[28], "","");
        }

        private void rel_15_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[29], "","");
        }

        private void rel_15_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[30], "","");
        }

        private void rel_16_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[31], "","");
        }

        private void rel_16_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[32], "","");
        }

        private void rel_17_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[33], "","");
        }

        private void rel_17_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[34], "","");
        }

        private void rel_18_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[35], "","");
        }

        private void rel_18_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[36], "","");
        }

        private void rel_19_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[37], "","");
        }

        private void rel_19_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[38], "","");
        }

        private void rel_20_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[39], "","");
        }

        private void rel_20_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[40], "","");
        }

        private void rel_21_43_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[41], "","");
        }

        private void rel_21_43_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[42], "","");
        }

        private void rel_22_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[43], "","");
        }

        private void rel_22_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[44], "","");
        }

        private void rel_23_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[45], "","");
        }

        private void rel_23_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[46], "","");
        }

        private void rel_24_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[47], "","");
        }

        private void rel_24_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[48], "","");
        }

        private void rel_25_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[49], "","");
        }

        private void rel_25_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[50], "","");
        }

        private void rel_26_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[51], "","");
        }

        private void rel_26_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[52], "","");
        }

        private void rel_27_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[53], "","");
        }

        private void rel_27_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[54], "","");
        }

        private void rel_28_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[55], "","");
        }

        private void rel_28_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[56], "","");
        }

        private void rel_29_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[57], "","");
        }

        private void rel_29_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[58], "","");
        }

        private void rel_30_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[59], "","");
        }

        private void rel_30_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[60], "","");
        }

        private void rel_33_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[65], "","");
        }

        private void rel_33_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[66], "","");
        }

        private void rel_34_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[67], "","");
        }

        private void rel_34_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[68], "","");
        }

        private void rel_35_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[69], "","");
        }

        private void rel_35_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[70], "","");
        }

        private void rel_36_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[71], "","");
        }

        private void rel_36_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[72], "","");
        }

        private void rel_37_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[73], "","");
        }

        private void rel_37_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[74], "","");
        }

        private void rel_39_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[77], "","");
        }

        private void rel_39_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[78], "","");
        }

        private void rel_40_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[79], "","");
        }

        private void rel_40_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[80], "","");
        }

        private void rel_41_on_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[81], "","");
        }

        private void rel_41_off_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[82], "","");
        }

        private void button42_on_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[83], "","");
        }

        private void button42_off_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[84], "","");
        }

        private void reset_all_rel_button_Click(object sender, EventArgs e)
        {
            serial_object.Send_protocol_message(Settings1.Default._COMMUNICATION_DIR_PORT1, Settings1.Default._ID_MT, Settings1.Default._ID_TFA,  serial_com.COMMAND_TYPE_NAMES[1], serial_com.RELAY_CODE_NAMES[0], "","");
        }

        private void Relay_form_FormClosing(object sender, FormClosingEventArgs e)
        {
            mainForm.machines_button.Enabled = true;
            mainForm.rescan_button.Enabled = true;
            mainForm.test_com_protocol_button.Enabled = true;
            mainForm.test_relays_button.Enabled = true;
            mainForm.port_connect.Enabled = true;
            mainForm.cord_button.Enabled = true;
        }

    }
}
