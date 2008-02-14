using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace rBank
{
    public class Customer: IComparable<Customer>
    {
        private string name;
        private int accountID;
        private IList<Account> accounts = new List<Account>();


        int IComparable<Customer>.CompareTo(Customer obj)
        {
            return this.name.CompareTo(obj.name);
        }

        public Customer(string str, int n){
            name = str;
            accountID = n;
            accounts.Add(new SavingAccount());
            accounts.Add(new CurrentAccount());
            accounts.Add(new USDAccount());
        }

        public void Summary(ListBox box)
        {
            box.Items.Clear();
            box.Items.Add(name);
            foreach(Account obj in accounts){
                box.Items.Add(obj.getName());
                box.Items.Add("$" + obj.Balance);
            }
            box.Show();
        }

        public void PrintAccounts(ListBox box)
        {
            box.Items.Clear();
            foreach (Account obj in accounts)
                box.Items.Add(obj);
            box.Show();
        }

        public string getAccountName(int i)
        {
            return accounts[i].getName();
        }

        public int Deposite(int accountType, double amount)
        {
            if (accountType < 0 || accountType >= accounts.Count)
            {
                Console.WriteLine("Invalid account tpye.");
                return -1;
            }
            return accounts[accountType].Deposit(amount);
        }

        public int Withdraw(int accountType, double amount)
        {
            if (accountType < 0 || accountType >= accounts.Count)
            {
                Console.WriteLine("Invalid account tpye.");
                return -1;
            }
            return accounts[accountType].Withdraw(amount);
        }

        public override string ToString()
        {
            return name;
        }
    }
}
