using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace rBank
{
    public partial class formOperate : Form
    {

        private Customer customer;
        private int accountIndex;
        private int operateType;
        public formOperate(Customer c, int i, int opt)
        {
            InitializeComponent();
            customer = c;
            accountIndex = i;
            operateType = opt;
        }

        private void buttonConfirm_Click(object sender, EventArgs e)
        {
            int ret = 0;
            double amount = 0;
            try{
                amount = Convert.ToDouble(textAmount.Text);
            }catch(System.Exception E){
                MessageBox.Show("Invalid input.", "Operation failed!", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                Console.WriteLine(E);
                this.Close();
            }
            switch (operateType)
            {
                case 0:
                    ret = customer.Withdraw(accountIndex, amount);
                    break;
                case 1:
                    ret = customer.Deposite(accountIndex, amount);
                    break;
            }
            if (ret != 0)
                MessageBox.Show("Invalid input or not enough balance.","Operation failed!",MessageBoxButtons.OK,MessageBoxIcon.Warning);
            this.Close();
        }

    }
}