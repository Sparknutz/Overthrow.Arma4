class WalletComponentClass: ScriptComponentClass
{}

class WalletComponent : ScriptComponent 
{
    [Attribute(uiwidget: UIWidgets.EditBox, params: "0 inf"), RplProp()]
    protected int m_iBalance;

    int GetBalance()
    {
        return m_iBalance;
    }

    void SetBalance(int amount)
    {
        if (!Replication.IsServer())
            return;
        m_iBalance = amount;
        if (m_iBalance < 0) m_iBalance = 0; // Prevent negative balance
        //Print("Wallet balance set to: " + m_iBalance);
        Replication.BumpMe();
    }

    void AddBalance(int amount)
    {
        if (!Replication.IsServer())
            return;
        m_iBalance += amount;
        if (m_iBalance < 0) m_iBalance = 0; // Prevent negative balance
        //Print("Wallet balance added: " + amount + ", new balance: " + m_iBalance);
        Replication.BumpMe();
    }

    void RemoveBalance(int amount)
    {
        if (!Replication.IsServer())
            return;
        m_iBalance -= amount;
        if (m_iBalance < 0) m_iBalance = 0; // Prevent negative balance
        //Print("Wallet balance removed: " + amount + ", new balance: " + m_iBalance);
        Replication.BumpMe();
    }
}