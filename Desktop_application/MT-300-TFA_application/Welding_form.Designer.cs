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
            this.SuspendLayout();
            // 
            // rpe_button
            // 
            this.rpe_button.Location = new System.Drawing.Point(455, 38);
            this.rpe_button.Name = "rpe_button";
            this.rpe_button.Size = new System.Drawing.Size(108, 23);
            this.rpe_button.TabIndex = 44;
            this.rpe_button.Text = "START RPE";
            this.rpe_button.UseVisualStyleBackColor = true;
            this.rpe_button.Click += new System.EventHandler(this.rpe_button_Click);
            // 
            // riso_button
            // 
            this.riso_button.Location = new System.Drawing.Point(332, 79);
            this.riso_button.Name = "riso_button";
            this.riso_button.Size = new System.Drawing.Size(108, 23);
            this.riso_button.TabIndex = 45;
            this.riso_button.Text = "START ALL-PE";
            this.riso_button.UseVisualStyleBackColor = true;
            this.riso_button.Click += new System.EventHandler(this.riso_button_Click);
            // 
            // riso_button2
            // 
            this.riso_button2.Location = new System.Drawing.Point(455, 79);
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
            this.iso_state_combobox.Location = new System.Drawing.Point(205, 79);
            this.iso_state_combobox.Name = "iso_state_combobox";
            this.iso_state_combobox.Size = new System.Drawing.Size(121, 21);
            this.iso_state_combobox.TabIndex = 47;
            // 
            // return_iso_nok_textbox
            // 
            this.return_iso_nok_textbox.Location = new System.Drawing.Point(111, 79);
            this.return_iso_nok_textbox.Name = "return_iso_nok_textbox";
            this.return_iso_nok_textbox.Size = new System.Drawing.Size(88, 20);
            this.return_iso_nok_textbox.TabIndex = 51;
            // 
            // return_iso_ok_textbox
            // 
            this.return_iso_ok_textbox.Location = new System.Drawing.Point(22, 79);
            this.return_iso_ok_textbox.Name = "return_iso_ok_textbox";
            this.return_iso_ok_textbox.Size = new System.Drawing.Size(83, 20);
            this.return_iso_ok_textbox.TabIndex = 50;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(138, 63);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(32, 13);
            this.label6.TabIndex = 49;
            this.label6.Text = "FAIL:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(54, 63);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(25, 13);
            this.label5.TabIndex = 48;
            this.label5.Text = "OK:";
            // 
            // mains_weld_button
            // 
            this.mains_weld_button.Location = new System.Drawing.Point(187, 123);
            this.mains_weld_button.Name = "mains_weld_button";
            this.mains_weld_button.Size = new System.Drawing.Size(126, 23);
            this.mains_weld_button.TabIndex = 52;
            this.mains_weld_button.Text = "START MAINS-WELD";
            this.mains_weld_button.UseVisualStyleBackColor = true;
            this.mains_weld_button.Click += new System.EventHandler(this.mains_weld_button_Click);
            // 
            // weld_pe_button
            // 
            this.weld_pe_button.Location = new System.Drawing.Point(332, 123);
            this.weld_pe_button.Name = "weld_pe_button";
            this.weld_pe_button.Size = new System.Drawing.Size(108, 23);
            this.weld_pe_button.TabIndex = 53;
            this.weld_pe_button.Text = "START WELD-PE";
            this.weld_pe_button.UseVisualStyleBackColor = true;
            this.weld_pe_button.Click += new System.EventHandler(this.weld_pe_button_Click);
            // 
            // mains_acc_button
            // 
            this.mains_acc_button.Location = new System.Drawing.Point(455, 123);
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
            this.riso_cont_checkbox.Location = new System.Drawing.Point(114, 127);
            this.riso_cont_checkbox.Name = "riso_cont_checkbox";
            this.riso_cont_checkbox.Size = new System.Drawing.Size(56, 17);
            this.riso_cont_checkbox.TabIndex = 55;
            this.riso_cont_checkbox.Text = "CONT";
            this.riso_cont_checkbox.UseVisualStyleBackColor = true;
            // 
            // UNL_RMS_button
            // 
            this.UNL_RMS_button.Location = new System.Drawing.Point(332, 171);
            this.UNL_RMS_button.Name = "UNL_RMS_button";
            this.UNL_RMS_button.Size = new System.Drawing.Size(108, 23);
            this.UNL_RMS_button.TabIndex = 56;
            this.UNL_RMS_button.Text = "START UNL RMS";
            this.UNL_RMS_button.UseVisualStyleBackColor = true;
            this.UNL_RMS_button.Click += new System.EventHandler(this.UNL_RMS_button_Click);
            // 
            // UNL_PEAK_button
            // 
            this.UNL_PEAK_button.Location = new System.Drawing.Point(455, 171);
            this.UNL_PEAK_button.Name = "UNL_PEAK_button";
            this.UNL_PEAK_button.Size = new System.Drawing.Size(108, 23);
            this.UNL_PEAK_button.TabIndex = 57;
            this.UNL_PEAK_button.Text = "START UNL PEAK";
            this.UNL_PEAK_button.UseVisualStyleBackColor = true;
            this.UNL_PEAK_button.Click += new System.EventHandler(this.UNL_PEAK_button_Click);
            // 
            // Welding_form
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(592, 296);
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
    }
}