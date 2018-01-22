namespace MT_300_TFA_application
{
    partial class Welding_form
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.rpe_button = new System.Windows.Forms.Button();
            this.riso_button = new System.Windows.Forms.Button();
            this.riso_button2 = new System.Windows.Forms.Button();
            this.iso_state_combobox = new System.Windows.Forms.ComboBox();
            this.return_iso_nok_textbox = new System.Windows.Forms.TextBox();
            this.return_iso_ok_textbox = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.mains_weld_button = new System.Windows.Forms.Button();
            this.weld_pe_button = new System.Windows.Forms.Button();
            this.mains_acc_button = new System.Windows.Forms.Button();
            this.riso_cont_checkbox = new System.Windows.Forms.CheckBox();
            this.UNL_RMS_button = new System.Windows.Forms.Button();
            this.UNL_PEAK_button = new System.Windows.Forms.Button();
            this.MainsAnalyzeButton = new System.Windows.Forms.Button();
            this.UL1textBox = new System.Windows.Forms.TextBox();
            this.UL2textBox = new System.Windows.Forms.TextBox();
            this.UL3textBox = new System.Windows.Forms.TextBox();
            this.IL1textBox = new System.Windows.Forms.TextBox();
            this.IL2textBox = new System.Windows.Forms.TextBox();
            this.IL3textBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.S3textBox = new System.Windows.Forms.TextBox();
            this.S2textBox = new System.Windows.Forms.TextBox();
            this.S1textBox = new System.Windows.Forms.TextBox();
            this.P3textBox = new System.Windows.Forms.TextBox();
            this.P2textBox = new System.Windows.Forms.TextBox();
            this.P1textBox = new System.Windows.Forms.TextBox();
            this.label15 = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.PF3textBox = new System.Windows.Forms.TextBox();
            this.PF2textBox = new System.Windows.Forms.TextBox();
            this.PF1textBox = new System.Windows.Forms.TextBox();
            this.S3PtextBox = new System.Windows.Forms.TextBox();
            this.PF3PtextBox = new System.Windows.Forms.TextBox();
            this.P3PtextBox = new System.Windows.Forms.TextBox();
            this.UNPEtextBox = new System.Windows.Forms.TextBox();
            this.UNPE = new System.Windows.Forms.Label();
            this.label18 = new System.Windows.Forms.Label();
            this.label19 = new System.Windows.Forms.Label();
            this.label20 = new System.Windows.Forms.Label();
            this.label21 = new System.Windows.Forms.Label();
            this.label22 = new System.Windows.Forms.Label();
            this.label23 = new System.Windows.Forms.Label();
            this.THD_IL3textBox = new System.Windows.Forms.TextBox();
            this.THD_IL2textBox = new System.Windows.Forms.TextBox();
            this.THD_IL1textBox = new System.Windows.Forms.TextBox();
            this.label24 = new System.Windows.Forms.Label();
            this.label25 = new System.Windows.Forms.Label();
            this.label26 = new System.Windows.Forms.Label();
            this.THD_ULN3textBox = new System.Windows.Forms.TextBox();
            this.THD_ULN2textBox = new System.Windows.Forms.TextBox();
            this.THD_ULN1textBox = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // rpe_button
            // 
            this.rpe_button.Location = new System.Drawing.Point(652, 29);
            this.rpe_button.Name = "rpe_button";
            this.rpe_button.Size = new System.Drawing.Size(108, 23);
            this.rpe_button.TabIndex = 44;
            this.rpe_button.Text = "START RPE";
            this.rpe_button.UseVisualStyleBackColor = true;
            this.rpe_button.Click += new System.EventHandler(this.rpe_button_Click);
            // 
            // riso_button
            // 
            this.riso_button.Location = new System.Drawing.Point(529, 70);
            this.riso_button.Name = "riso_button";
            this.riso_button.Size = new System.Drawing.Size(108, 23);
            this.riso_button.TabIndex = 45;
            this.riso_button.Text = "START ALL-PE";
            this.riso_button.UseVisualStyleBackColor = true;
            this.riso_button.Click += new System.EventHandler(this.riso_button_Click);
            // 
            // riso_button2
            // 
            this.riso_button2.Location = new System.Drawing.Point(652, 70);
            this.riso_button2.Name = "riso_button2";
            this.riso_button2.Size = new System.Drawing.Size(108, 23);
            this.riso_button2.TabIndex = 46;
            this.riso_button2.Text = "START ONE-PE";
            this.riso_button2.UseVisualStyleBackColor = true;
            this.riso_button2.Click += new System.EventHandler(this.riso_button2_Click);
            // 
            // iso_state_combobox
            // 
            this.iso_state_combobox.FormattingEnabled = true;
            this.iso_state_combobox.Items.AddRange(new object[] {
            "ISO OK",
            "L1 - N",
            "L1 - PE",
            "L3 - PE",
            "N - PE",
            "L2,L3 - N",
            "L1,N -PE",
            "L1 - L2"});
            this.iso_state_combobox.Location = new System.Drawing.Point(290, 70);
            this.iso_state_combobox.Name = "iso_state_combobox";
            this.iso_state_combobox.Size = new System.Drawing.Size(121, 21);
            this.iso_state_combobox.TabIndex = 47;
            // 
            // return_iso_nok_textbox
            // 
            this.return_iso_nok_textbox.Location = new System.Drawing.Point(196, 70);
            this.return_iso_nok_textbox.Name = "return_iso_nok_textbox";
            this.return_iso_nok_textbox.Size = new System.Drawing.Size(88, 20);
            this.return_iso_nok_textbox.TabIndex = 51;
            // 
            // return_iso_ok_textbox
            // 
            this.return_iso_ok_textbox.Location = new System.Drawing.Point(107, 70);
            this.return_iso_ok_textbox.Name = "return_iso_ok_textbox";
            this.return_iso_ok_textbox.Size = new System.Drawing.Size(83, 20);
            this.return_iso_ok_textbox.TabIndex = 50;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(223, 54);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(32, 13);
            this.label6.TabIndex = 49;
            this.label6.Text = "FAIL:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(139, 54);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(25, 13);
            this.label5.TabIndex = 48;
            this.label5.Text = "OK:";
            // 
            // mains_weld_button
            // 
            this.mains_weld_button.Location = new System.Drawing.Point(290, 114);
            this.mains_weld_button.Name = "mains_weld_button";
            this.mains_weld_button.Size = new System.Drawing.Size(126, 23);
            this.mains_weld_button.TabIndex = 52;
            this.mains_weld_button.Text = "START MAINS-WELD";
            this.mains_weld_button.UseVisualStyleBackColor = true;
            this.mains_weld_button.Click += new System.EventHandler(this.mains_weld_button_Click);
            // 
            // weld_pe_button
            // 
            this.weld_pe_button.Location = new System.Drawing.Point(529, 114);
            this.weld_pe_button.Name = "weld_pe_button";
            this.weld_pe_button.Size = new System.Drawing.Size(108, 23);
            this.weld_pe_button.TabIndex = 53;
            this.weld_pe_button.Text = "START WELD-PE";
            this.weld_pe_button.UseVisualStyleBackColor = true;
            this.weld_pe_button.Click += new System.EventHandler(this.weld_pe_button_Click);
            // 
            // mains_acc_button
            // 
            this.mains_acc_button.Location = new System.Drawing.Point(652, 114);
            this.mains_acc_button.Name = "mains_acc_button";
            this.mains_acc_button.Size = new System.Drawing.Size(108, 23);
            this.mains_acc_button.TabIndex = 54;
            this.mains_acc_button.Text = "START MAINS-AC";
            this.mains_acc_button.UseVisualStyleBackColor = true;
            this.mains_acc_button.Click += new System.EventHandler(this.mains_acc_button_Click);
            // 
            // riso_cont_checkbox
            // 
            this.riso_cont_checkbox.AutoSize = true;
            this.riso_cont_checkbox.Location = new System.Drawing.Point(196, 120);
            this.riso_cont_checkbox.Name = "riso_cont_checkbox";
            this.riso_cont_checkbox.Size = new System.Drawing.Size(56, 17);
            this.riso_cont_checkbox.TabIndex = 55;
            this.riso_cont_checkbox.Text = "CONT";
            this.riso_cont_checkbox.UseVisualStyleBackColor = true;
            // 
            // UNL_RMS_button
            // 
            this.UNL_RMS_button.Location = new System.Drawing.Point(529, 162);
            this.UNL_RMS_button.Name = "UNL_RMS_button";
            this.UNL_RMS_button.Size = new System.Drawing.Size(108, 23);
            this.UNL_RMS_button.TabIndex = 56;
            this.UNL_RMS_button.Text = "START UNL RMS";
            this.UNL_RMS_button.UseVisualStyleBackColor = true;
            this.UNL_RMS_button.Click += new System.EventHandler(this.UNL_RMS_button_Click);
            // 
            // UNL_PEAK_button
            // 
            this.UNL_PEAK_button.Location = new System.Drawing.Point(652, 162);
            this.UNL_PEAK_button.Name = "UNL_PEAK_button";
            this.UNL_PEAK_button.Size = new System.Drawing.Size(108, 23);
            this.UNL_PEAK_button.TabIndex = 57;
            this.UNL_PEAK_button.Text = "START UNL PEAK";
            this.UNL_PEAK_button.UseVisualStyleBackColor = true;
            this.UNL_PEAK_button.Click += new System.EventHandler(this.UNL_PEAK_button_Click);
            // 
            // MainsAnalyzeButton
            // 
            this.MainsAnalyzeButton.Location = new System.Drawing.Point(327, 205);
            this.MainsAnalyzeButton.Name = "MainsAnalyzeButton";
            this.MainsAnalyzeButton.Size = new System.Drawing.Size(153, 23);
            this.MainsAnalyzeButton.TabIndex = 58;
            this.MainsAnalyzeButton.Text = "START MAINS ANALYZE";
            this.MainsAnalyzeButton.UseVisualStyleBackColor = true;
            this.MainsAnalyzeButton.Click += new System.EventHandler(this.MainsAnalyzeButton_Click);
            // 
            // UL1textBox
            // 
            this.UL1textBox.Location = new System.Drawing.Point(67, 248);
            this.UL1textBox.Name = "UL1textBox";
            this.UL1textBox.Size = new System.Drawing.Size(66, 20);
            this.UL1textBox.TabIndex = 59;
            // 
            // UL2textBox
            // 
            this.UL2textBox.Location = new System.Drawing.Point(67, 274);
            this.UL2textBox.Name = "UL2textBox";
            this.UL2textBox.Size = new System.Drawing.Size(66, 20);
            this.UL2textBox.TabIndex = 60;
            // 
            // UL3textBox
            // 
            this.UL3textBox.Location = new System.Drawing.Point(67, 300);
            this.UL3textBox.Name = "UL3textBox";
            this.UL3textBox.Size = new System.Drawing.Size(66, 20);
            this.UL3textBox.TabIndex = 61;
            // 
            // IL1textBox
            // 
            this.IL1textBox.Location = new System.Drawing.Point(167, 248);
            this.IL1textBox.Name = "IL1textBox";
            this.IL1textBox.Size = new System.Drawing.Size(66, 20);
            this.IL1textBox.TabIndex = 62;
            // 
            // IL2textBox
            // 
            this.IL2textBox.Location = new System.Drawing.Point(167, 274);
            this.IL2textBox.Name = "IL2textBox";
            this.IL2textBox.Size = new System.Drawing.Size(66, 20);
            this.IL2textBox.TabIndex = 63;
            // 
            // IL3textBox
            // 
            this.IL3textBox.Location = new System.Drawing.Point(167, 300);
            this.IL3textBox.Name = "IL3textBox";
            this.IL3textBox.Size = new System.Drawing.Size(66, 20);
            this.IL3textBox.TabIndex = 64;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(22, 251);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 13);
            this.label1.TabIndex = 65;
            this.label1.Text = "ULN1";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(22, 277);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(35, 13);
            this.label2.TabIndex = 66;
            this.label2.Text = "ULN2";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(22, 303);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(35, 13);
            this.label3.TabIndex = 67;
            this.label3.Text = "ULN3";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(139, 251);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(22, 13);
            this.label4.TabIndex = 68;
            this.label4.Text = "IL1";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(139, 277);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(22, 13);
            this.label7.TabIndex = 69;
            this.label7.Text = "IL2";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(139, 303);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(22, 13);
            this.label8.TabIndex = 70;
            this.label8.Text = "IL3";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(356, 303);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(20, 13);
            this.label9.TabIndex = 82;
            this.label9.Text = "S3";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(356, 277);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(20, 13);
            this.label10.TabIndex = 81;
            this.label10.Text = "S2";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(356, 251);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(20, 13);
            this.label11.TabIndex = 80;
            this.label11.Text = "S1";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(246, 303);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(20, 13);
            this.label12.TabIndex = 79;
            this.label12.Text = "P3";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(246, 277);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(20, 13);
            this.label13.TabIndex = 78;
            this.label13.Text = "P2";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(246, 251);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(20, 13);
            this.label14.TabIndex = 77;
            this.label14.Text = "P1";
            // 
            // S3textBox
            // 
            this.S3textBox.Location = new System.Drawing.Point(382, 300);
            this.S3textBox.Name = "S3textBox";
            this.S3textBox.Size = new System.Drawing.Size(66, 20);
            this.S3textBox.TabIndex = 76;
            // 
            // S2textBox
            // 
            this.S2textBox.Location = new System.Drawing.Point(382, 274);
            this.S2textBox.Name = "S2textBox";
            this.S2textBox.Size = new System.Drawing.Size(66, 20);
            this.S2textBox.TabIndex = 75;
            // 
            // S1textBox
            // 
            this.S1textBox.Location = new System.Drawing.Point(382, 248);
            this.S1textBox.Name = "S1textBox";
            this.S1textBox.Size = new System.Drawing.Size(66, 20);
            this.S1textBox.TabIndex = 74;
            // 
            // P3textBox
            // 
            this.P3textBox.Location = new System.Drawing.Point(270, 300);
            this.P3textBox.Name = "P3textBox";
            this.P3textBox.Size = new System.Drawing.Size(66, 20);
            this.P3textBox.TabIndex = 73;
            // 
            // P2textBox
            // 
            this.P2textBox.Location = new System.Drawing.Point(270, 274);
            this.P2textBox.Name = "P2textBox";
            this.P2textBox.Size = new System.Drawing.Size(66, 20);
            this.P2textBox.TabIndex = 72;
            // 
            // P1textBox
            // 
            this.P1textBox.Location = new System.Drawing.Point(270, 248);
            this.P1textBox.Name = "P1textBox";
            this.P1textBox.Size = new System.Drawing.Size(66, 20);
            this.P1textBox.TabIndex = 71;
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(465, 303);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(26, 13);
            this.label15.TabIndex = 88;
            this.label15.Text = "PF3";
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(465, 277);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(26, 13);
            this.label16.TabIndex = 87;
            this.label16.Text = "PF2";
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(465, 251);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(26, 13);
            this.label17.TabIndex = 86;
            this.label17.Text = "PF1";
            // 
            // PF3textBox
            // 
            this.PF3textBox.Location = new System.Drawing.Point(497, 300);
            this.PF3textBox.Name = "PF3textBox";
            this.PF3textBox.Size = new System.Drawing.Size(66, 20);
            this.PF3textBox.TabIndex = 85;
            // 
            // PF2textBox
            // 
            this.PF2textBox.Location = new System.Drawing.Point(497, 274);
            this.PF2textBox.Name = "PF2textBox";
            this.PF2textBox.Size = new System.Drawing.Size(66, 20);
            this.PF2textBox.TabIndex = 84;
            // 
            // PF1textBox
            // 
            this.PF1textBox.Location = new System.Drawing.Point(497, 248);
            this.PF1textBox.Name = "PF1textBox";
            this.PF1textBox.Size = new System.Drawing.Size(66, 20);
            this.PF1textBox.TabIndex = 83;
            // 
            // S3PtextBox
            // 
            this.S3PtextBox.Location = new System.Drawing.Point(382, 326);
            this.S3PtextBox.Name = "S3PtextBox";
            this.S3PtextBox.Size = new System.Drawing.Size(66, 20);
            this.S3PtextBox.TabIndex = 89;
            // 
            // PF3PtextBox
            // 
            this.PF3PtextBox.Location = new System.Drawing.Point(497, 326);
            this.PF3PtextBox.Name = "PF3PtextBox";
            this.PF3PtextBox.Size = new System.Drawing.Size(66, 20);
            this.PF3PtextBox.TabIndex = 90;
            // 
            // P3PtextBox
            // 
            this.P3PtextBox.Location = new System.Drawing.Point(270, 326);
            this.P3PtextBox.Name = "P3PtextBox";
            this.P3PtextBox.Size = new System.Drawing.Size(66, 20);
            this.P3PtextBox.TabIndex = 91;
            // 
            // UNPEtextBox
            // 
            this.UNPEtextBox.Location = new System.Drawing.Point(67, 327);
            this.UNPEtextBox.Name = "UNPEtextBox";
            this.UNPEtextBox.Size = new System.Drawing.Size(66, 20);
            this.UNPEtextBox.TabIndex = 92;
            // 
            // UNPE
            // 
            this.UNPE.AutoSize = true;
            this.UNPE.Location = new System.Drawing.Point(22, 329);
            this.UNPE.Name = "UNPE";
            this.UNPE.Size = new System.Drawing.Size(37, 13);
            this.UNPE.TabIndex = 93;
            this.UNPE.Text = "UNPE";
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(356, 330);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(27, 13);
            this.label18.TabIndex = 94;
            this.label18.Text = "S3P";
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Location = new System.Drawing.Point(244, 330);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(27, 13);
            this.label19.TabIndex = 95;
            this.label19.Text = "P3P";
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Location = new System.Drawing.Point(465, 329);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(33, 13);
            this.label20.TabIndex = 96;
            this.label20.Text = "PF3P";
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Location = new System.Drawing.Point(712, 303);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(48, 13);
            this.label21.TabIndex = 108;
            this.label21.Text = "THD IL3";
            // 
            // label22
            // 
            this.label22.AutoSize = true;
            this.label22.Location = new System.Drawing.Point(712, 277);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(48, 13);
            this.label22.TabIndex = 107;
            this.label22.Text = "THD IL2";
            // 
            // label23
            // 
            this.label23.AutoSize = true;
            this.label23.Location = new System.Drawing.Point(712, 251);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(48, 13);
            this.label23.TabIndex = 106;
            this.label23.Text = "THD IL1";
            // 
            // THD_IL3textBox
            // 
            this.THD_IL3textBox.Location = new System.Drawing.Point(766, 300);
            this.THD_IL3textBox.Name = "THD_IL3textBox";
            this.THD_IL3textBox.Size = new System.Drawing.Size(66, 20);
            this.THD_IL3textBox.TabIndex = 105;
            // 
            // THD_IL2textBox
            // 
            this.THD_IL2textBox.Location = new System.Drawing.Point(766, 274);
            this.THD_IL2textBox.Name = "THD_IL2textBox";
            this.THD_IL2textBox.Size = new System.Drawing.Size(66, 20);
            this.THD_IL2textBox.TabIndex = 104;
            // 
            // THD_IL1textBox
            // 
            this.THD_IL1textBox.Location = new System.Drawing.Point(766, 248);
            this.THD_IL1textBox.Name = "THD_IL1textBox";
            this.THD_IL1textBox.Size = new System.Drawing.Size(66, 20);
            this.THD_IL1textBox.TabIndex = 103;
            // 
            // label24
            // 
            this.label24.AutoSize = true;
            this.label24.Location = new System.Drawing.Point(577, 303);
            this.label24.Name = "label24";
            this.label24.Size = new System.Drawing.Size(61, 13);
            this.label24.TabIndex = 102;
            this.label24.Text = "THD ULN3";
            // 
            // label25
            // 
            this.label25.AutoSize = true;
            this.label25.Location = new System.Drawing.Point(577, 277);
            this.label25.Name = "label25";
            this.label25.Size = new System.Drawing.Size(61, 13);
            this.label25.TabIndex = 101;
            this.label25.Text = "THD ULN2";
            // 
            // label26
            // 
            this.label26.AutoSize = true;
            this.label26.Location = new System.Drawing.Point(577, 251);
            this.label26.Name = "label26";
            this.label26.Size = new System.Drawing.Size(61, 13);
            this.label26.TabIndex = 100;
            this.label26.Text = "THD ULN1";
            // 
            // THD_ULN3textBox
            // 
            this.THD_ULN3textBox.Location = new System.Drawing.Point(640, 300);
            this.THD_ULN3textBox.Name = "THD_ULN3textBox";
            this.THD_ULN3textBox.Size = new System.Drawing.Size(66, 20);
            this.THD_ULN3textBox.TabIndex = 99;
            // 
            // THD_ULN2textBox
            // 
            this.THD_ULN2textBox.Location = new System.Drawing.Point(640, 274);
            this.THD_ULN2textBox.Name = "THD_ULN2textBox";
            this.THD_ULN2textBox.Size = new System.Drawing.Size(66, 20);
            this.THD_ULN2textBox.TabIndex = 98;
            // 
            // THD_ULN1textBox
            // 
            this.THD_ULN1textBox.Location = new System.Drawing.Point(640, 248);
            this.THD_ULN1textBox.Name = "THD_ULN1textBox";
            this.THD_ULN1textBox.Size = new System.Drawing.Size(66, 20);
            this.THD_ULN1textBox.TabIndex = 97;
            // 
            // Welding_form
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(874, 364);
            this.Controls.Add(this.label21);
            this.Controls.Add(this.label22);
            this.Controls.Add(this.label23);
            this.Controls.Add(this.THD_IL3textBox);
            this.Controls.Add(this.THD_IL2textBox);
            this.Controls.Add(this.THD_IL1textBox);
            this.Controls.Add(this.label24);
            this.Controls.Add(this.label25);
            this.Controls.Add(this.label26);
            this.Controls.Add(this.THD_ULN3textBox);
            this.Controls.Add(this.THD_ULN2textBox);
            this.Controls.Add(this.THD_ULN1textBox);
            this.Controls.Add(this.label20);
            this.Controls.Add(this.label19);
            this.Controls.Add(this.label18);
            this.Controls.Add(this.UNPE);
            this.Controls.Add(this.UNPEtextBox);
            this.Controls.Add(this.P3PtextBox);
            this.Controls.Add(this.PF3PtextBox);
            this.Controls.Add(this.S3PtextBox);
            this.Controls.Add(this.label15);
            this.Controls.Add(this.label16);
            this.Controls.Add(this.label17);
            this.Controls.Add(this.PF3textBox);
            this.Controls.Add(this.PF2textBox);
            this.Controls.Add(this.PF1textBox);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.label13);
            this.Controls.Add(this.label14);
            this.Controls.Add(this.S3textBox);
            this.Controls.Add(this.S2textBox);
            this.Controls.Add(this.S1textBox);
            this.Controls.Add(this.P3textBox);
            this.Controls.Add(this.P2textBox);
            this.Controls.Add(this.P1textBox);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.IL3textBox);
            this.Controls.Add(this.IL2textBox);
            this.Controls.Add(this.IL1textBox);
            this.Controls.Add(this.UL3textBox);
            this.Controls.Add(this.UL2textBox);
            this.Controls.Add(this.UL1textBox);
            this.Controls.Add(this.MainsAnalyzeButton);
            this.Controls.Add(this.UNL_PEAK_button);
            this.Controls.Add(this.UNL_RMS_button);
            this.Controls.Add(this.riso_cont_checkbox);
            this.Controls.Add(this.mains_acc_button);
            this.Controls.Add(this.weld_pe_button);
            this.Controls.Add(this.mains_weld_button);
            this.Controls.Add(this.return_iso_nok_textbox);
            this.Controls.Add(this.return_iso_ok_textbox);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.iso_state_combobox);
            this.Controls.Add(this.riso_button2);
            this.Controls.Add(this.riso_button);
            this.Controls.Add(this.rpe_button);
            this.Name = "Welding_form";
            this.Text = "Machines_form";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Welding_form_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button rpe_button;
        private System.Windows.Forms.Button riso_button;
        private System.Windows.Forms.Button riso_button2;
        private System.Windows.Forms.ComboBox iso_state_combobox;
        private System.Windows.Forms.TextBox return_iso_nok_textbox;
        private System.Windows.Forms.TextBox return_iso_ok_textbox;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button mains_weld_button;
        private System.Windows.Forms.Button weld_pe_button;
        private System.Windows.Forms.Button mains_acc_button;
        private System.Windows.Forms.CheckBox riso_cont_checkbox;
        private System.Windows.Forms.Button UNL_RMS_button;
        private System.Windows.Forms.Button UNL_PEAK_button;
        private System.Windows.Forms.Button MainsAnalyzeButton;
        private System.Windows.Forms.TextBox UL1textBox;
        private System.Windows.Forms.TextBox UL2textBox;
        private System.Windows.Forms.TextBox UL3textBox;
        private System.Windows.Forms.TextBox IL1textBox;
        private System.Windows.Forms.TextBox IL2textBox;
        private System.Windows.Forms.TextBox IL3textBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.TextBox S3textBox;
        private System.Windows.Forms.TextBox S2textBox;
        private System.Windows.Forms.TextBox S1textBox;
        private System.Windows.Forms.TextBox P3textBox;
        private System.Windows.Forms.TextBox P2textBox;
        private System.Windows.Forms.TextBox P1textBox;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.TextBox PF3textBox;
        private System.Windows.Forms.TextBox PF2textBox;
        private System.Windows.Forms.TextBox PF1textBox;
        private System.Windows.Forms.TextBox S3PtextBox;
        private System.Windows.Forms.TextBox PF3PtextBox;
        private System.Windows.Forms.TextBox P3PtextBox;
        private System.Windows.Forms.TextBox UNPEtextBox;
        private System.Windows.Forms.Label UNPE;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.Label label22;
        private System.Windows.Forms.Label label23;
        private System.Windows.Forms.TextBox THD_IL3textBox;
        private System.Windows.Forms.TextBox THD_IL2textBox;
        private System.Windows.Forms.TextBox THD_IL1textBox;
        private System.Windows.Forms.Label label24;
        private System.Windows.Forms.Label label25;
        private System.Windows.Forms.Label label26;
        private System.Windows.Forms.TextBox THD_ULN3textBox;
        private System.Windows.Forms.TextBox THD_ULN2textBox;
        private System.Windows.Forms.TextBox THD_ULN1textBox;
    }
}