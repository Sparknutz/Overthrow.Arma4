[EPF_ComponentSaveDataType(WalletComponent), BaseContainerProps()]
class SPARK_WalletComponentSaveDataClass : EPF_ComponentSaveDataClass
{
};

[EDF_DbName.Automatic()]
class SPARK_WalletComponentSaveData : EPF_ComponentSaveData
{
	int m_iMoney;

//------------------------------------------------------------------------------------------------
	override EPF_EReadResult ReadFrom(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
	{
		WalletComponent wallet = WalletComponent.Cast(component);
        if (!wallet)
            return EPF_EReadResult.ERROR;
        m_iMoney = wallet.GetBalance();
        Print("Saving wallet with money: " + m_iMoney);
        return EPF_EReadResult.OK;
	}

//------------------------------------------------------------------------------------------------
	override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
	{
		WalletComponent wallet = WalletComponent.Cast(component);
        if (!wallet)
            return EPF_EApplyResult.ERROR;
        wallet.SetBalance(m_iMoney);
        Print("Loaded wallet with money: " + m_iMoney);
        return EPF_EApplyResult.OK;
	}

//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EPF_ComponentSaveData other)
	{
		SPARK_WalletComponentSaveData otherData = SPARK_WalletComponentSaveData.Cast(other);
		return m_iMoney == otherData.m_iMoney;
	}
};