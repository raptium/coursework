using System;
using System.Collections.Generic;
using System.Text;
using Account;

namespace Customer
{
    public class Customer
    {
        private string name;
        private int accountID;
        private Account.Account[] accounts;

        public Customer(string str, int n){
            name = str;
            accountID = n;
            accounts = new Account.Account[2];
            accounts[0] = new SavingAccount();
            accounts[1] = new CurrentAccount();
        }

        public void PrintCustomer()
        {
            Console.WriteLine("Account holder: " + name);
            Console.WriteLine("Account ID: " + accountID);
            Console.WriteLine("Account type:");
            for (int i = 0; i < accounts.Length; i++)
                accounts[i].PrintAccount();
            Console.WriteLine("");
        }

        public int Deposite(int accountType, double amount)
        {
            if (accountType < 0 || accountType >= accounts.Length)
            {
                Console.WriteLine("Invalid account tpye.");
                return -1;
            }
            return accounts[accountType].Deposit(amount);
        }

        public int Withdraw(int accountType, double amount)
        {
            if (accountType < 0 || accountType >= accounts.Length)
            {
                Console.WriteLine("Invalid account tpye.");
                return -1;
            }
            return accounts[accountType].Withdraw(amount);
        }
    }
}
