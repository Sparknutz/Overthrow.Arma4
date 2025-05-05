class ATMUI : SCR_ScriptedWidgetComponent
{
    // UI elements
    protected TextWidget m_BalanceDisplay;
    protected TextWidget m_MessageDisplay;
    protected Widget m_NumberPad;
    protected ButtonWidget m_DepositButton;
    protected ButtonWidget m_WithdrawButton;
    protected ButtonWidget m_ViewBalanceButton;
    protected ButtonWidget m_CancelButton;
    protected Widget m_ConfirmationPanel;
    protected TextWidget m_ConfirmationMessage;
    protected ButtonWidget m_YesButton;
    protected ButtonWidget m_NoButton;
    protected TextWidget m_AmountDisplay;

    // Variables
    protected string m_CurrentInput;
    protected string m_SelectedAction;
    protected ATMComponent m_ATM;

    // Initialization
    void OnPostInit(IEntity owner)
    {
        // Find and store references to UI elements
        m_BalanceDisplay = TextWidget.Cast(m_wRoot.FindAnyWidget("BalanceDisplay"));
        m_MessageDisplay = TextWidget.Cast(m_wRoot.FindAnyWidget("MessageDisplay"));
        m_NumberPad = m_wRoot.FindAnyWidget("NumberPad");
        m_DepositButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("DepositButton"));
        m_WithdrawButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("WithdrawButton"));
        m_ViewBalanceButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("ViewBalanceButton"));
        m_CancelButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("CancelButton"));
        m_ConfirmationPanel = m_wRoot.FindAnyWidget("ConfirmationPanel");
        m_ConfirmationMessage = TextWidget.Cast(m_wRoot.FindAnyWidget("ConfirmationMessage"));
        m_YesButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("YesButton"));
        m_NoButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("NoButton"));
        m_AmountDisplay = TextWidget.Cast(m_wRoot.FindAnyWidget("AmountDisplay"));

        // Hide confirmation panel by default
        m_ConfirmationPanel.SetVisible(false);

        // Set up button click handlers
        SCR_InputButtonComponent depositAction = SCR_InputButtonComponent.Cast(m_DepositButton.FindHandler(SCR_InputButtonComponent));
        depositAction.m_OnActivated.Insert(OnDepositClicked);

        SCR_InputButtonComponent withdrawAction = SCR_InputButtonComponent.Cast(m_WithdrawButton.FindHandler(SCR_InputButtonComponent));
        withdrawAction.m_OnActivated.Insert(OnWithdrawClicked);

        SCR_InputButtonComponent viewBalanceAction = SCR_InputButtonComponent.Cast(m_ViewBalanceButton.FindHandler(SCR_InputButtonComponent));
        viewBalanceAction.m_OnActivated.Insert(OnViewBalanceClicked);

        SCR_InputButtonComponent cancelAction = SCR_InputButtonComponent.Cast(m_CancelButton.FindHandler(SCR_InputButtonComponent));
        cancelAction.m_OnActivated.Insert(OnCancelClicked);

        SCR_InputButtonComponent yesAction = SCR_InputButtonComponent.Cast(m_YesButton.FindHandler(SCR_InputButtonComponent));
        yesAction.m_OnActivated.Insert(OnConfirmYes);

        SCR_InputButtonComponent noAction = SCR_InputButtonComponent.Cast(m_NoButton.FindHandler(SCR_InputButtonComponent));
        noAction.m_OnActivated.Insert(OnConfirmNo);

        // Set up number pad buttons
        for (int i = 0; i <= 9; i++)
        {
            ButtonWidget numberButton = ButtonWidget.Cast(m_NumberPad.FindAnyWidget("Number" + i));
            if (numberButton)
            {
                SCR_InputButtonComponent action = SCR_InputButtonComponent.Cast(numberButton.FindHandler(SCR_InputButtonComponent));
                action.m_OnActivated.Insert(OnNumberClicked);
            }
        }

        ButtonWidget clearButton = ButtonWidget.Cast(m_NumberPad.FindAnyWidget("Clear"));
        if (clearButton)
        {
            SCR_InputButtonComponent clearAction = SCR_InputButtonComponent.Cast(clearButton.FindHandler(SCR_InputButtonComponent));
            clearAction.m_OnActivated.Insert(OnClearClicked);
        }

        // Initialize variables
        m_CurrentInput = "";
        m_SelectedAction = "";
    }
	
	void SetVisible(bool visible)
	{
	    m_wRoot.SetVisible(visible);
	}
	
	void SetWalletBalance(int balance)
	{
	    TextWidget walletBalanceWidget = TextWidget.Cast(m_wRoot.FindAnyWidget("WalletBalance"));
	    if (walletBalanceWidget)
	        walletBalanceWidget.SetText("Wallet: $" + balance);
	}
	
	void SetATMBalance(int balance)
	{
	    TextWidget atmBalanceWidget = TextWidget.Cast(m_wRoot.FindAnyWidget("ATMBalance"));
	    if (atmBalanceWidget)
	        atmBalanceWidget.SetText("ATM: $" + balance);
	}

    // Set the ATM component reference
    void SetATMComponent(ATMComponent atm)
    {
        m_ATM = atm;
    }

    // Handler for number pad clicks
    void OnNumberClicked(Widget w)
    {
        string name = w.GetName();
        int number = name.Substring(6, 1).ToInt(); // Extract number from "NumberX"
        m_CurrentInput += number.ToString();
        UpdateAmountDisplay();
        PlayClickSound();
    }

    // Handler for clear button
    void OnClearClicked()
    {
        m_CurrentInput = "";
        UpdateAmountDisplay();
        PlayClickSound();
    }

    // Update the amount display
    void UpdateAmountDisplay()
    {
        m_AmountDisplay.SetText(m_CurrentInput);
    }

    // Handler for deposit button
    void OnDepositClicked()
    {
        m_SelectedAction = "deposit";
        ShowConfirmationDialog();
        PlayClickSound();
    }

    // Handler for withdraw button
    void OnWithdrawClicked()
    {
        m_SelectedAction = "withdraw";
        ShowConfirmationDialog();
        PlayClickSound();
    }

    // Handler for view balance button
    void OnViewBalanceClicked()
    {
        if (m_ATM)
        {
            string playerId = SCR_PlayerController.GetLocalPlayerId().ToString();
            m_ATM.RpcAsk_CheckBalance(playerId);
        }
        PlayClickSound();
    }

    // Handler for cancel button
    void OnCancelClicked()
    {
        // Close the UI
        m_wRoot.SetVisible(false);
        PlayClickSound();
    }

    // Show confirmation dialog with capitalized action
	void ShowConfirmationDialog()
	{
	    if (m_CurrentInput == "")
	    {
	        m_MessageDisplay.SetText("Please enter an amount.");
	        PlayErrorSound();
	        return;
	    }
	
	    
	    m_ConfirmationMessage.SetText( " $" + m_CurrentInput + "?");
	    m_ConfirmationPanel.SetVisible(true);
	}

    

    // Handler for confirmation Yes
    void OnConfirmYes()
    {
        if (m_ATM)
        {
            string playerId = SCR_PlayerController.GetLocalPlayerId().ToString();
            int amount = m_CurrentInput.ToInt();
            if (m_SelectedAction == "deposit")
            {
                m_ATM.RpcAsk_Deposit(playerId, amount);
            }
            else if (m_SelectedAction == "withdraw")
            {
                m_ATM.RpcAsk_Withdraw(playerId, amount);
            }
        }
        m_CurrentInput = "";
        UpdateAmountDisplay();
        m_ConfirmationPanel.SetVisible(false);
        PlaySuccessSound();
    }

    // Handler for confirmation No
    void OnConfirmNo()
    {
        m_CurrentInput = "";
        UpdateAmountDisplay();
        m_ConfirmationPanel.SetVisible(false);
        PlayClickSound();
    }

    // Placeholder sound functions
    void PlayClickSound()
    {
        // Play click sound here
    }

    void PlaySuccessSound()
    {
        // Play success sound here
    }

    void PlayErrorSound()
    {
        // Play error sound here
    }

    // Public methods for ATMComponent to update UI
    void UpdateBalance(int balance)
    {
        m_BalanceDisplay.SetText("Current Balance: $" + balance);
    }

    void ShowMessage(string message)
    {
        m_MessageDisplay.SetText(message);
    }
}