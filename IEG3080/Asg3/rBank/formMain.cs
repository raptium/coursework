using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace rBank
{
    

    public partial class formMain : Form
    {
        private List<Customer> customerList = new List<Customer>();

        public formMain()
        {
            InitializeComponent();
            listAccounts.Hide();
            listCustomers.Hide();
            listSummary.Hide();
            customerList.Add(new Customer("James Bond", 7));
            customerList.Add(new Customer("Goldfinger", 8));
            customerList.Sort();
        }

        private void FormMain_Load(object sender, EventArgs e)
        {

        }

        private void listToolStripMenuItem_Click(object sender, EventArgs e)
        {
            listCustomers.Items.Clear();
            foreach (Customer obj in customerList)
                listCustomers.Items.Add(obj);
            listCustomers.Show();
            listCustomers.SelectedIndex = 0;
        }

        private void listCustomers_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listCustomers.SelectedIndex < 0)
                return;
            Customer obj = customerList[listCustomers.SelectedIndex];
            obj.PrintAccounts(listAccounts);
            obj.Summary(listSummary);
        }

        private void homeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            listAccounts.Hide();
            listCustomers.Hide();
            listSummary.Hide();
        }

        private void listAccounts_MouseUp(object sender, MouseEventArgs e)
        {
            int i = listAccounts.IndexFromPoint(e.X, e.Y);
            if (i >= 0 && e.Button == MouseButtons.Right)
            {
                listAccounts.SelectedIndex = i;
                Customer c = customerList[listCustomers.SelectedIndex];
                formAccount popUp = new formAccount(c, i);
                DialogResult ret = popUp.ShowDialog();
                c.PrintAccounts(listAccounts);
                c.Summary(listSummary);
            }
        }
    }
}