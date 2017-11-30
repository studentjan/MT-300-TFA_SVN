namespace MT_300_TFA_application
{
    partial class Cord_form
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
            this.polarity_test_start_button = new System.Windows.Forms.Button();
            this.meas_in_progress_ind = new System.Windows.Forms.Button();
            this.Cable_type_comboBox = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.resistance_return_value = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.Cable_state_comboBox = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.fail_return_textbox = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // polarity_test_start_button
            // 
            this.polarity_test_start_button.Location = new System.Drawing.Point(354, 72);
            this.polarity_test_start_button.Name = "polarity_test_start_button";
            this.polarity_test_start_button.Size = new System.Drawing.Size(89, 23);
            this.polarity_test_start_button.TabIndex = 0;
            this.polarity_test_start_button.Text = "START";
            this.polarity_test_start_button.UseVisualStyleBackColor = true;
            this.polarity_test_start_button.Click += new System.EventHandler(this.polarity_test_start_button_Click);
            // 
            // meas_in_progress_ind
            // 
            this.meas_in_progress_ind.Location = new System.Drawing.Point(588, 72);
            this.meas_in_progress_ind.Name = "meas_in_progress_ind";
            this.meas_in_progress_ind.Size = new System.Drawing.Size(20, 23);
            this.meas_in_progress_ind.TabIndex = 1;
            this.meas_in_progress_ind.UseVisualStyleBackColor = true;
            // 
            // Cable_type_comboBox
            // 
            this.Cable_type_comboBox.FormattingEnabled = true;
            this.Cable_type_comboBox.Items.AddRange(new object[] {
            "1 PHASE",
            "3 PHASE"});
            this.Cable_type_comboBox.Location = new System.Drawing.Point(58, 74);
            this.Cable_type_comboBox.Name = "Cable_type_comboBox";
            this.Cable_type_comboBox.Size = new System.Drawing.Size(121, 21);
            this.Cable_type_comboBox.TabIndex = 2;
            this.Cable_type_comboBox.SelectedIndexChanged += new System.EventHandler(this.Cable_type_comboBox_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(82, 56);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(72, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "CABLE TYPE";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(465, 77);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(117, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "MEAS IN PROGRESS:";
            // 
            // resistance_return_value
            // 
            this.resistance_return_value.Location = new System.Drawing.Point(487, 165);
            this.resistance_return_value.Name = "resistance_return_value";
            this.resistance_return_value.Size = new System.Drawing.Size(121, 20);
            this.resistance_return_value.TabIndex = 5;
            this.resistance_return_value.TextChanged += new System.EventHandler(this.resistance_return_value_TextChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(418, 137);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(124, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "RETURN RESISTANCE";
            // 
            // Cable_state_comboBox
            // 
            this.Cable_state_comboBox.FormattingEnabled = true;
            this.Cable_state_comboBox.Items.AddRange(new object[] {
            "CABLE OK",
            "L - N CROSSED",
            "N - L CROSSED",
            "N - OPEN",
            "L1 - N SHORTED",
            "L1 - PE SHORTED",
            "L1 - L3 CROSSED",
            "1P_L - N SHORTED",
            "1P_L OPEN",
            "L1-L2, L3-N CROSSED",
            "PE-L1, L2-N SHORTED",
            "ALL CROSSED",
            "PE-L1 CROSSED AND OPEN"});
            this.Cable_state_comboBox.Location = new System.Drawing.Point(58, 167);
            this.Cable_state_comboBox.Name = "Cable_state_comboBox";
            this.Cable_state_comboBox.Size = new System.Drawing.Size(121, 21);
            this.Cable_state_comboBox.TabIndex = 7;
            this.Cable_state_comboBox.SelectedIndexChanged += new System.EventHandler(this.Cable_state_comboBox_SelectedIndexChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(82, 151);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(79, 13);
            this.label4.TabIndex = 8;
            this.label4.Text = "CABLE STATE";
            // 
            // fail_return_textbox
            // 
            this.fail_return_textbox.Location = new System.Drawing.Point(487, 191);
            this.fail_return_textbox.Name = "fail_return_textbox";
            this.fail_return_textbox.Size = new System.Drawing.Size(121, 20);
            this.fail_return_textbox.TabIndex = 9;
            this.fail_return_textbox.TextChanged += new System.EventHandler(this.fail_return_textbox_TextChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(418, 170);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(25, 13);
            this.label5.TabIndex = 10;
            this.label5.Text = "OK:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(418, 194);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(32, 13);
            this.label6.TabIndex = 11;
            this.label6.Text = "FAIL:";
            // 
            // Cord_form
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(644, 287);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.fail_return_textbox);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.Cable_state_comboBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.resistance_return_value);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.Cable_type_comboBox);
            this.Controls.Add(this.meas_in_progress_ind);
            this.Controls.Add(this.polarity_test_start_button);
            this.Name = "Cord_form";
            this.Text = "Form2";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button polarity_test_start_button;
        private System.Windows.Forms.Button meas_in_progress_ind;
        private System.Windows.Forms.ComboBox Cable_type_comboBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox resistance_return_value;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox Cable_state_comboBox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox fail_return_textbox;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
    }
}