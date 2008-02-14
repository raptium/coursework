namespace rBank
{
    partial class formMain
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
            this.menuMain = new System.Windows.Forms.MenuStrip();
            this.homeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.listToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.listCustomers = new System.Windows.Forms.ListBox();
            this.listAccounts = new System.Windows.Forms.ListBox();
            this.listSummary = new System.Windows.Forms.ListBox();
            this.menuMain.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuMain
            // 
            this.menuMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.homeToolStripMenuItem,
            this.listToolStripMenuItem});
            this.menuMain.Location = new System.Drawing.Point(0, 0);
            this.menuMain.Name = "menuMain";
            this.menuMain.Size = new System.Drawing.Size(292, 24);
            this.menuMain.TabIndex = 0;
            this.menuMain.Text = "menuMain";
            // 
            // homeToolStripMenuItem
            // 
            this.homeToolStripMenuItem.Name = "homeToolStripMenuItem";
            this.homeToolStripMenuItem.Size = new System.Drawing.Size(46, 20);
            this.homeToolStripMenuItem.Text = "Home";
            this.homeToolStripMenuItem.Click += new System.EventHandler(this.homeToolStripMenuItem_Click);
            // 
            // listToolStripMenuItem
            // 
            this.listToolStripMenuItem.Name = "listToolStripMenuItem";
            this.listToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.listToolStripMenuItem.Text = "List";
            this.listToolStripMenuItem.Click += new System.EventHandler(this.listToolStripMenuItem_Click);
            // 
            // listCustomers
            // 
            this.listCustomers.FormattingEnabled = true;
            this.listCustomers.Location = new System.Drawing.Point(12, 27);
            this.listCustomers.Name = "listCustomers";
            this.listCustomers.Size = new System.Drawing.Size(268, 108);
            this.listCustomers.TabIndex = 1;
            this.listCustomers.SelectedIndexChanged += new System.EventHandler(this.listCustomers_SelectedIndexChanged);
            // 
            // listAccounts
            // 
            this.listAccounts.FormattingEnabled = true;
            this.listAccounts.Location = new System.Drawing.Point(13, 142);
            this.listAccounts.Name = "listAccounts";
            this.listAccounts.Size = new System.Drawing.Size(267, 82);
            this.listAccounts.TabIndex = 2;
            this.listAccounts.MouseUp += new System.Windows.Forms.MouseEventHandler(this.listAccounts_MouseUp);
            // 
            // listSummary
            // 
            this.listSummary.FormattingEnabled = true;
            this.listSummary.Location = new System.Drawing.Point(13, 230);
            this.listSummary.Name = "listSummary";
            this.listSummary.Size = new System.Drawing.Size(267, 95);
            this.listSummary.TabIndex = 3;
            // 
            // formMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 336);
            this.Controls.Add(this.listSummary);
            this.Controls.Add(this.listAccounts);
            this.Controls.Add(this.listCustomers);
            this.Controls.Add(this.menuMain);
            this.MainMenuStrip = this.menuMain;
            this.Name = "formMain";
            this.Text = "rBank";
            this.Load += new System.EventHandler(this.FormMain_Load);
            this.menuMain.ResumeLayout(false);
            this.menuMain.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuMain;
        private System.Windows.Forms.ToolStripMenuItem homeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem listToolStripMenuItem;
        private System.Windows.Forms.ListBox listCustomers;
        private System.Windows.Forms.ListBox listAccounts;
        private System.Windows.Forms.ListBox listSummary;
    }
}

