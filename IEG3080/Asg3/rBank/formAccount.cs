using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace rBank
{
    public partial class formAccount : Form
    {
        private Customer customer;
        private int accountIndex;

        public formAccount(Customer c, int j)
        {
            InitializeComponent();
            customer = c;
            accountIndex = j;
            this.Text = customer + "'s " + customer.getAccountName(accountIndex);
            
        }

        private void withdrawToolStripMenuItem_Click(object sender, EventArgs e)
        {
            formOperate popUp = new formOperate(customer, accountIndex, 0);
            DialogResult ret = popUp.ShowDialog();
            this.Close();
        }

        private void depositToolStripMenuItem_Click(object sender, EventArgs e)
        {
            formOperate popUp = new formOperate(customer, accountIndex, 1);
            DialogResult ret = popUp.ShowDialog();
            this.Close();
        }
    }
}