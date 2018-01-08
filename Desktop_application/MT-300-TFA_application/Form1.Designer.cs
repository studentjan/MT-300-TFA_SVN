namespace MT_300_TFA_application
{
    public partial class Form1
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
            this.components = new System.ComponentModel.Container();
            this.port_connect = new System.Windows.Forms.Button();
            this.avalible_ports_combobox = new System.Windows.Forms.ComboBox();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.AdapterCOM_status = new System.Windows.Forms.Button();
            this.terminal_textbox = new System.Windows.Forms.TextBox();
            this.terminal_textbox_send = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.test_com_protocol_button = new System.Windows.Forms.Button();
            this.cord_button = new System.Windows.Forms.Button();
            this.test_relays_button = new System.Windows.Forms.Button();
            this.clear_send_button = new System.Windows.Forms.Button();
            this.clear_receive_button = new System.Windows.Forms.Button();
            this.rescan_button = new System.Windows.Forms.Button();
            this.machines_button = new System.Windows.Forms.Button();
            this.weld_button = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // port_connect
            // 
            this.port_connect.Location = new System.Drawing.Point(208, 25);
            this.port_connect.Name = "port_connect";
            this.port_connect.Size = new System.Drawing.Size(114, 21);
            this.port_connect.TabIndex = 0;
            this.port_connect.Text = "CONNECT";
            this.port_connect.UseVisualStyleBackColor = true;
            this.port_connect.Click += new System.EventHandler(this.port_connect_Click);
            // 
            // avalible_ports_combobox
            // 
            this.avalible_ports_combobox.FormattingEnabled = true;
            this.avalible_ports_combobox.Location = new System.Drawing.Point(10, 25);
            this.avalible_ports_combobox.Name = "avalible_ports_combobox";
            this.avalible_ports_combobox.Size = new System.Drawing.Size(119, 21);
            this.avalible_ports_combobox.TabIndex = 1;
            // 
            // AdapterCOM_status
            // 
            this.AdapterCOM_status.Location = new System.Drawing.Point(158, 24);
            this.AdapterCOM_status.Name = "AdapterCOM_status";
            this.AdapterCOM_status.Size = new System.Drawing.Size(20, 22);
            this.AdapterCOM_status.TabIndex = 2;
            this.AdapterCOM_status.UseVisualStyleBackColor = true;
            // 
            // terminal_textbox
            // 
            this.terminal_textbox.Location = new System.Drawing.Point(466, 192);
            this.terminal_textbox.Multiline = true;
            this.terminal_textbox.Name = "terminal_textbox";
            this.terminal_textbox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.terminal_textbox.Size = new System.Drawing.Size(433, 224);
            this.terminal_textbox.TabIndex = 15;
            // 
            // terminal_textbox_send
            // 
            this.terminal_textbox_send.Location = new System.Drawing.Point(12, 192);
            this.terminal_textbox_send.Multiline = true;
            this.terminal_textbox_send.Name = "terminal_textbox_send";
            this.terminal_textbox_send.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.terminal_textbox_send.Size = new System.Drawing.Size(433, 224);
            this.terminal_textbox_send.TabIndex = 16;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(658, 168);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(127, 13);
            this.label1.TabIndex = 17;
            this.label1.Text = "RECIEVED COMMANDS";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(174, 168);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(102, 13);
            this.label2.TabIndex = 18;
            this.label2.Text = "SENT COMMANDS";
            // 
            // test_com_protocol_button
            // 
            this.test_com_protocol_button.Location = new System.Drawing.Point(767, 12);
            this.test_com_protocol_button.Name = "test_com_protocol_button";
            this.test_com_protocol_button.Size = new System.Drawing.Size(118, 23);
            this.test_com_protocol_button.TabIndex = 19;
            this.test_com_protocol_button.Text = "TEST PROTOCOL";
            this.test_com_protocol_button.UseVisualStyleBackColor = true;
            this.test_com_protocol_button.Click += new System.EventHandler(this.test_com_protocol_button_Click);
            // 
            // cord_button
            // 
            this.cord_button.Location = new System.Drawing.Point(532, 12);
            this.cord_button.Name = "cord_button";
            this.cord_button.Size = new System.Drawing.Size(105, 23);
            this.cord_button.TabIndex = 20;
            this.cord_button.Text = "CORD TEST";
            this.cord_button.UseVisualStyleBackColor = true;
            this.cord_button.Click += new System.EventHandler(this.cord_button_Click);
            // 
            // test_relays_button
            // 
            this.test_relays_button.Location = new System.Drawing.Point(643, 12);
            this.test_relays_button.Name = "test_relays_button";
            this.test_relays_button.Size = new System.Drawing.Size(118, 23);
            this.test_relays_button.TabIndex = 21;
            this.test_relays_button.Text = "TEST RELAYS";
            this.test_relays_button.UseVisualStyleBackColor = true;
            this.test_relays_button.Click += new System.EventHandler(this.test_relays_button_Click);
            // 
            // clear_send_button
            // 
            this.clear_send_button.Location = new System.Drawing.Point(10, 163);
            this.clear_send_button.Name = "clear_send_button";
            this.clear_send_button.Size = new System.Drawing.Size(75, 23);
            this.clear_send_button.TabIndex = 22;
            this.clear_send_button.Text = "CLEAR";
            this.clear_send_button.UseVisualStyleBackColor = true;
            this.clear_send_button.Click += new System.EventHandler(this.clear_send_button_Click);
            // 
            // clear_receive_button
            // 
            this.clear_receive_button.Location = new System.Drawing.Point(466, 163);
            this.clear_receive_button.Name = "clear_receive_button";
            this.clear_receive_button.Size = new System.Drawing.Size(75, 23);
            this.clear_receive_button.TabIndex = 23;
            this.clear_receive_button.Text = "CLEAR";
            this.clear_receive_button.UseVisualStyleBackColor = true;
            this.clear_receive_button.Click += new System.EventHandler(this.clear_receive_button_Click);
            // 
            // rescan_button
            // 
            this.rescan_button.Location = new System.Drawing.Point(15, 64);
            this.rescan_button.Name = "rescan_button";
            this.rescan_button.Size = new System.Drawing.Size(114, 21);
            this.rescan_button.TabIndex = 24;
            this.rescan_button.Text = "RESCAN";
            this.rescan_button.UseVisualStyleBackColor = true;
            this.rescan_button.Click += new System.EventHandler(this.rescan_button_Click);
            // 
            // machines_button
            // 
            this.machines_button.Location = new System.Drawing.Point(451, 12);
            this.machines_button.Name = "machines_button";
            this.machines_button.Size = new System.Drawing.Size(75, 23);
            this.machines_button.TabIndex = 25;
            this.machines_button.Text = "MACHINES";
            this.machines_button.UseVisualStyleBackColor = true;
            this.machines_button.Click += new System.EventHandler(this.machines_button_Click);
            // 
            // weld_button
            // 
            this.weld_button.Location = new System.Drawing.Point(370, 12);
            this.weld_button.Name = "weld_button";
            this.weld_button.Size = new System.Drawing.Size(75, 23);
            this.weld_button.TabIndex = 26;
            this.weld_button.Text = "WELDING";
            this.weld_button.UseVisualStyleBackColor = true;
            this.weld_button.Click += new System.EventHandler(this.weld_button_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(940, 428);
            this.Controls.Add(this.weld_button);
            this.Controls.Add(this.machines_button);
            this.Controls.Add(this.rescan_button);
            this.Controls.Add(this.clear_receive_button);
            this.Controls.Add(this.clear_send_button);
            this.Controls.Add(this.test_relays_button);
            this.Controls.Add(this.cord_button);
            this.Controls.Add(this.test_com_protocol_button);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.terminal_textbox_send);
            this.Controls.Add(this.terminal_textbox);
            this.Controls.Add(this.AdapterCOM_status);
            this.Controls.Add(this.avalible_ports_combobox);
            this.Controls.Add(this.port_connect);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.Button port_connect;
        private System.Windows.Forms.ComboBox avalible_ports_combobox;
        public System.IO.Ports.SerialPort serialPort1;
        public System.Windows.Forms.Button AdapterCOM_status;
        private System.Windows.Forms.TextBox terminal_textbox;
        private System.Windows.Forms.TextBox terminal_textbox_send;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.Button test_com_protocol_button;
        public System.Windows.Forms.Button cord_button;
        public System.Windows.Forms.Button test_relays_button;
        private System.Windows.Forms.Button clear_send_button;
        private System.Windows.Forms.Button clear_receive_button;
        public System.Windows.Forms.Button rescan_button;
        public System.Windows.Forms.Button machines_button;
        public System.Windows.Forms.Button weld_button;
    }
}

