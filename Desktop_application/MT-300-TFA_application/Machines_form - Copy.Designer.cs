namespace MT_300_TFA_application
{
    partial class Machines_form
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
            this.test_contactors_button = new System.Windows.Forms.Button();
            this.Select_measurement_comboBox = new System.Windows.Forms.ComboBox();
            this.button1 = new System.Windows.Forms.Button();
            this.rpe_button = new System.Windows.Forms.Button();
            this.riso_button = new System.Windows.Forms.Button();
            this.riso_button2 = new System.Windows.Forms.Button();
            this.iso_state_combobox = new System.Windows.Forms.ComboBox();
            this.return_iso_nok_textbox = new System.Windows.Forms.TextBox();
            this.return_iso_ok_textbox = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // test_contactors_button
            // 
            this.test_contactors_button.Location = new System.Drawing.Point(366, 47);
            this.test_contactors_button.Name = "test_contactors_button";
            this.test_contactors_button.Size = new System.Drawing.Size(130, 23);
            this.test_contactors_button.TabIndex = 41;
            this.test_contactors_button.Text = "TEST CONTACTORS";
            this.test_contactors_button.UseVisualStyleBackColor = true;
            this.test_contactors_button.Click += new System.EventHandler(this.test_contactors_button_Click);
            // 
            // Select_measurement_comboBox
            // 
            this.Select_measurement_comboBox.FormattingEnabled = true;
            this.Select_measurement_comboBox.Items.AddRange(new object[] {
            "L1-N",
            "L2-N",
            "L3-N",
            "L1-L2",
            "L2-L3",
            "L1-L3"});
            this.Select_measurement_comboBox.Location = new System.Drawing.Point(48, 49);
            this.Select_measurement_comboBox.Name = "Select_measurement_comboBox";
            this.Select_measurement_comboBox.Size = new System.Drawing.Size(121, 21);
            this.Select_measurement_comboBox.TabIndex = 42;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(217, 47);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(108, 23);
            this.button1.TabIndex = 43;
            this.button1.Text = "START URES";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // rpe_button
            // 
            this.rpe_button.Location = new System.Drawing.Point(217, 97);
            this.rpe_button.Name = "rpe_button";
            this.rpe_button.Size = new System.Drawing.Size(108, 23);
            this.rpe_button.TabIndex = 44;
            this.rpe_button.Text = "START RPE";
            this.rpe_button.UseVisualStyleBackColor = true;
            this.rpe_button.Click += new System.EventHandler(this.rpe_button_Click);
            // 
            // riso_button
            // 
            this.riso_button.Location = new System.Drawing.Point(217, 145);
            this.riso_button.Name = "riso_button";
            this.riso_button.Size = new System.Drawing.Size(108, 23);
            this.riso_button.TabIndex = 45;
            this.riso_button.Text = "START ALL-PE";
            this.riso_button.UseVisualStyleBackColor = true;
            this.riso_button.Click += new System.EventHandler(this.riso_button_Click);
            // 
            // riso_button2
            // 
            this.riso_button2.Location = new System.Drawing.Point(217, 195);
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
            this.iso_state_combobox.Location = new System.Drawing.Point(48, 145);
            this.iso_state_combobox.Name = "iso_state_combobox";
            this.iso_state_combobox.Size = new System.Drawing.Size(121, 21);
            this.iso_state_combobox.TabIndex = 47;
            // 
            // return_iso_nok_textbox
            // 
            this.return_iso_nok_textbox.Location = new System.Drawing.Point(126, 229);
            this.return_iso_nok_textbox.Name = "return_iso_nok_textbox";
            this.return_iso_nok_textbox.Size = new System.Drawing.Size(88, 20);
            this.return_iso_nok_textbox.TabIndex = 51;
            // 
            // return_iso_ok_textbox
            // 
            this.return_iso_ok_textbox.Location = new System.Drawing.Point(23, 229);
            this.return_iso_ok_textbox.Name = "return_iso_ok_textbox";
            this.return_iso_ok_textbox.Size = new System.Drawing.Size(83, 20);
            this.return_iso_ok_textbox.TabIndex = 50;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(155, 213);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(32, 13);
            this.label6.TabIndex = 49;
            this.label6.Text = "FAIL:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(45, 213);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(25, 13);
            this.label5.TabIndex = 48;
            this.label5.Text = "OK:";
            // 
            // Machines_form
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(592, 296);
            this.Controls.Add(this.return_iso_nok_textbox);
            this.Controls.Add(this.return_iso_ok_textbox);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.iso_state_combobox);
            this.Controls.Add(this.riso_button2);
            this.Controls.Add(this.riso_button);
            this.Controls.Add(this.rpe_button);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.Select_measurement_comboBox);
            this.Controls.Add(this.test_contactors_button);
            this.Name = "Machines_form";
            this.Text = "Machines_form";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Machines_form_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button test_contactors_button;
        private System.Windows.Forms.ComboBox Select_measurement_comboBox;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button rpe_button;
        private System.Windows.Forms.Button riso_button;
        private System.Windows.Forms.Button riso_button2;
        private System.Windows.Forms.ComboBox iso_state_combobox;
        private System.Windows.Forms.TextBox return_iso_nok_textbox;
        private System.Windows.Forms.TextBox return_iso_ok_textbox;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
    }
}