class ATMComponentClass: ScriptComponentClass
{}
class ATMComponent : ScriptComponent
{
    [Attribute()]
    ResourceName m_UILayout; // Path to the UI layout file (e.g., "layouts/ATM.layout")

    [RplProp()]
    protected string m_CurrentUserId; // ID of the player currently using the ATM

    protected ref ATMUI m_UI; // Reference to the UI instance on the client

    // Temporary map to store balances (replace with database later)
    protected ref map<string, int> m_Balances = new map<string, int>();

    // Called when a player interacts with the ATM (e.g., via an action key)
    void OnPlayerInteract(string playerId)
    {
        if (Replication.IsServer())
        {
            m_CurrentUserId = playerId;
            Rpc(RpcDo_OpenUI, playerId); // Tell the client to open the UI
        }
    }

    // Client-side RPC to open the UI
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_OpenUI(string playerId)
    {
        if (playerId != SCR_PlayerController.GetLocalPlayerId().ToString())
            return;

        WorkspaceWidget workspace = GetGame().GetWorkspace();
        m_UI = ATMUI.Cast(workspace.CreateWidgets(m_UILayout).GetHandler(0));
        if (m_UI)
        {
            m_UI.SetVisible(true);

            // Set initial wallet balance
            IEntity player = SCR_PlayerController.GetLocalControlledEntity();
            if (player)
            {
                WalletComponent wallet = WalletComponent.Cast(player.FindComponent(WalletComponent));
                if (wallet)
                    m_UI.SetWalletBalance(wallet.GetBalance());
            }

            // Request initial ATM balance
            Rpc(RpcAsk_CheckBalance, playerId);
        }
    }

    // Server-side RPC to process deposit
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_Deposit(string playerId, int amount)
    {
        if (!Replication.IsServer() || playerId != m_CurrentUserId || amount <= 0)
            return;

        int balance = GetPlayerBalance(playerId);
        int intPlayerId = playerId.ToInt();
        IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(intPlayerId);
        if (!player)
            return;

        WalletComponent wallet = WalletComponent.Cast(player.FindComponent(WalletComponent));
        if (!wallet || wallet.GetBalance() < amount)
            return;

        wallet.RemoveBalance(amount);
        balance += amount;
        SetPlayerBalance(playerId, balance);

        Rpc(RpcDo_UpdateBalances, playerId, balance, wallet.GetBalance());
    }

    // Server-side RPC to process withdrawal
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_Withdraw(string playerId, int amount)
    {
        if (!Replication.IsServer() || playerId != m_CurrentUserId || amount <= 0)
            return;

        int balance = GetPlayerBalance(playerId);
        if (balance < amount)
            return;

        int intPlayerId = playerId.ToInt();
        IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(intPlayerId);
        if (!player)
            return;

        WalletComponent wallet = WalletComponent.Cast(player.FindComponent(WalletComponent));
        if (!wallet)
            return;

        balance -= amount;
        SetPlayerBalance(playerId, balance);
        wallet.AddBalance(amount);

        Rpc(RpcDo_UpdateBalances, playerId, balance, wallet.GetBalance());
    }

    // Server-side RPC to check balance
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_CheckBalance(string playerId)
    {
        if (!Replication.IsServer() || playerId != m_CurrentUserId)
            return;

        int balance = GetPlayerBalance(playerId);
        int intPlayerId = playerId.ToInt();
        IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(intPlayerId);
        int walletBalance = 0;
        if (player)
        {
            WalletComponent wallet = WalletComponent.Cast(player.FindComponent(WalletComponent));
            if (wallet)
                walletBalance = wallet.GetBalance();
        }

        Rpc(RpcDo_UpdateBalances, playerId, balance, walletBalance);
    }

    // Client-side RPC to update UI balances
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_UpdateBalances(string playerId, int atmBalance, int walletBalance)
    {
        if (playerId != SCR_PlayerController.GetLocalPlayerId().ToString() || !m_UI)
            return;

        m_UI.SetATMBalance(atmBalance);
        m_UI.SetWalletBalance(walletBalance);
    }

    // Helper to retrieve player balance (temporary)
    protected int GetPlayerBalance(string playerId)
    {
        if (m_Balances.Contains(playerId))
            return m_Balances[playerId];
        else
            return 0;
    }

    // Helper to set player balance (temporary)
    protected void SetPlayerBalance(string playerId, int balance)
    {
        m_Balances[playerId] = balance;
    }

    // Ensure replication is set up
    override void EOnInit(IEntity owner)
    {
        SetEventMask(owner, EntityEvent.INIT);
        if (Replication.IsClient())
            m_CurrentUserId = "";
    }
}