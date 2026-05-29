//
// [ CTR_EV.CPP ]
//

#include "stdst.h"

#include <control.h>
#include <db_eng.h>
#include <menubar.h>
#include <toolbar.h>
#include <events.h>
#include <cstr.h>
#include <info.h>

#if !defined(__TEST__)
#include <stm2.h>
extern STM2 *g_STM2;
#endif

extern DB_ENGINE  *g_dbEngine;
extern CFG        *g_cfg;

EVENT_TYPE CONTROLLER::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case E_DEVICE:
    case E_CONTROLLER:
        // Turn the ??? on or off.
        switch (event.rawCode)
        {
        case D_OFF:
        case D_ON:
            {
                state = event.rawCode;
                break;
            }
        case UE_CONFIG_ON:
            {
                if (event.region.left)
                {
                    View->WConfigMenu->woFlags &= ~WOF_NON_SELECTABLE;
                    View->WActivateConfig->Event(UI_EVENT(L_UP, 0));
                    View->WActivateConfig->woFlags |= WOF_NON_SELECTABLE;
                    if (event.region.right)
                        View->WChangePasswd->woFlags &= ~WOF_NON_SELECTABLE;
                    else
                        View->WChangePasswd->woFlags |= WOF_NON_SELECTABLE;
                }
                else
                {
                    View->WConfigMenu->woFlags |= WOF_NON_SELECTABLE;
                    View->WActivateConfig->woFlags &= ~WOF_NON_SELECTABLE;
                }
                View->WConfigMenu->Event(UI_EVENT(S_REDISPLAY, 0));
                break;
            }
        case UE_EXTENSION_ON:
            {
				extern BOOL g_extAreChangeable;
                if (event.region.left)
                {
                    View->WExtMenu->woFlags &= ~WOF_NON_SELECTABLE;
                    View->WActivateExt->Event(UI_EVENT(L_DOWN, 0));
                    View->WActivateExt->woFlags |= WOF_NON_SELECTABLE;
					g_extAreChangeable = event.region.right;
                }
                else
                {
                    View->WExtMenu->woFlags |= WOF_NON_SELECTABLE;
                    View->WActivateExt->woFlags &= ~WOF_NON_SELECTABLE;
                }
                View->WExtMenu->Event(UI_EVENT(S_REDISPLAY, 0));
                break;
            }
        case UE_AUTO_ON:
            {
                if (event.region.left)
                    View->WBNumber->woFlags |= WOF_NON_SELECTABLE;
                else
                    View->WBNumber->woFlags &= ~WOF_NON_SELECTABLE;
                View->WBNumber->Event(UI_EVENT(S_REDISPLAY, 0));
                break;
            }
        case UE_LOCK:
            {
                int cNum = event.region.left/CLUSTER_SIZE;
                int bNum = event.region.left%CLUSTER_SIZE;
				if (!RTEngine->GetLocked(cNum, bNum))
                {
                    // go STORE (to avoid lost of data) and then force RT_ENGINE::LOCK
                    // only DTMF (because its precedence over the other)
					RTEngine->SetLocked(cNum, bNum, TRUE);
					RTEngine->ResetStateCount(cNum, bNum); // 2.21.1 Build 5 gc|hj
					RTEngine->SetFSs(cNum, bNum, RT_ENGINE::LOCK, RT_ENGINE::STORE);
				}
                break;
            }
        case UE_UNLOCK:
            {
                int cNum = event.region.left/CLUSTER_SIZE;
				int bNum = event.region.left%CLUSTER_SIZE;

				// unforce RT_ENGINE::LOCK only if is Locked
				if (RTEngine->GetLocked(cNum, bNum))
				{
					RTEngine->SetLocked(cNum, bNum, FALSE);
                }
                break;
            }
        case UE_SPY:
			{
				int cNum, bNum;

				// un-spy the others, first the data port !!!
				RTEngine->SaveLastSpyBooth(); // save

				if (RTEngine->GetLastSpyBooth() != -1)
				{
					// un-spy
					cNum = RTEngine->GetLastSpyBooth()/CLUSTER_SIZE;
					bNum = RTEngine->GetLastSpyBooth()%CLUSTER_SIZE;

					RTEngine->SetDataPortSpy(cNum, (BYTE)0);

					if (!event.region.right)
					{ 	// external ?
						// unlock
						RTEngine->SetLocked(cNum, bNum, FALSE);
					}
				}

				// spy
				RTEngine->SetSpyBooth(event.region.left);
				if (RTEngine->GetSpyBooth() != -1)
				{
					// spy
					cNum = RTEngine->GetSpyBooth()/CLUSTER_SIZE;
					bNum = RTEngine->GetSpyBooth()%CLUSTER_SIZE;

					// --- check to see if it is internal or external
					if (event.region.right)
					{
						// is it internal ?
						BYTE generalPort = RTEngine->GetGeneralPort();
						generalPort |= GP_INLINE; // deactivate in line
						RTEngine->SetGeneralPort(generalPort);
					}
					else
					{
						// go STORE (to avoid lost of data) and then force RT_ENGINE::LOCK
						// only DTMF (because its precedence over the other)
						RTEngine->SetLocked(cNum, bNum, TRUE);
						RTEngine->ResetStateCount(cNum, bNum); // 2.21.1 Build 5 gc|hj
						RTEngine->SetFSs(cNum, bNum, RT_ENGINE::LOCK, RT_ENGINE::STORE);
					}

					RTEngine->SetDataPortSpy(cNum, BYTE(1 << bNum));
				}
				else
				{
					// both internal and external, althought external never deactivate it !!!
					BYTE generalPort = RTEngine->GetGeneralPort();
					generalPort &= (~GP_INLINE); // deactivate in line
					RTEngine->SetGeneralPort(generalPort);
				}

				Refresh = TRUE;
				break;
			}
		case UE_COM_FROM_UIW_SIMULA:
			{
				int bNum, cNum;
                bNum = event.modifiers & 0xFF;
                cNum = bNum/CLUSTER_SIZE;
                bNum   = bNum%CLUSTER_SIZE;
				if (RTEngine->GetSimula(cNum, bNum))
					break; // avoid simula running calls

				if
				(
					(
						RTEngine->GetPulseFS(cNum, bNum) > RT_ENGINE::ONHOOK ||
						RTEngine->GetToneFS(cNum, bNum)  > RT_ENGINE::ONHOOK
					) &&
					RTEngine->IsAvailable(cNum)
				)
				{
					break; // avoid real running calls
				}

                const UINT MIN_DIGITS  = 6;  // for a phone number
                if (strlen((char *) event.data) < MIN_DIGITS)
                    break; // We need a minimum of digits
                int flags = event.modifiers >> 8;
                RTEngine->SetSimula(cNum, bNum, TRUE);

				// we clean some view members for security
				View->resetData(cNum, bNum);
				RTEngine->SetSimulaPhone(cNum, bNum, (PHONE const &) *(char *)event.data);

				// we use RT_ENGINE::ONHOOK to signalize the first simula
				RTEngine->SetNoReceipt(cNum, bNum, (flags & RECEIPT_ON)?FALSE:TRUE);
				RTEngine->SetNoStatistics(cNum, bNum, (flags & ACCUM_ON)  ?FALSE:TRUE);

				RTEngine->SetFSs(cNum, bNum, RT_ENGINE::ONHOOK, RT_ENGINE::SIMULA);
				break;
			}
		case UE_HANG_FROM_UIW_SIMULA:
            {
                int bNum, cNum;
                cNum = event.region.left/CLUSTER_SIZE;
				bNum   = event.region.left%CLUSTER_SIZE;

				RTEngine->ResetSimulaPhone(cNum, bNum); // clean !!!
                // go simula: prevents false hangs
				if (RTEngine->GetSimula(cNum, bNum))
                {
					RTEngine->ResetStateCount(cNum, bNum); // 2.21.1 Build 5 gc|hj
					RTEngine->SetFSs(cNum, bNum, RT_ENGINE::LOCK, RT_ENGINE::STORE);
				}
                break;
            }
        case UE_CANCEL_FROM_UIW_SIMULA:
            {
                int cNum, bNum;
				for (cNum=0; cNum<g_cfg->ACTIVE_CLUSTERS; cNum++)
                    for (bNum=0; bNum<CLUSTER_SIZE; bNum++)
                    {
						if (RTEngine->GetSimula(cNum, bNum))
                        {
							RTEngine->SetSimula(cNum, bNum, FALSE);
							RTEngine->SetFSs(cNum, bNum, RT_ENGINE::LOCK, RT_ENGINE::LOCK);
							if (!RTEngine->IsAvailable(cNum))
							{
								RTEngine->SetToneFS(cNum, bNum, RT_ENGINE::NOPHONE);
							}

							RTEngine->SetNoReceipt(cNum, bNum, FALSE);
							RTEngine->SetNoStatistics(cNum, bNum, FALSE);

							RTEngine->SetPrePaid(cNum, bNum, FALSE);

                            View->resetData(cNum, bNum);
                        }
                    }
                break;
            }
        case UE_INTER:
            {
                int cNum = event.modifiers/CLUSTER_SIZE;
				int bNum = event.modifiers%CLUSTER_SIZE;

				double value = RTEngine->GetPreValue(cNum, bNum);

				// Generate receipt
				if (g_cfg->GENERATE_PREPAID_RECEIPT)
				{
					printPrePaidReceipt(event.modifiers+1, value);
					if (g_cfg->DOUBLE_PREPAID_RECEIPT)
					{
						printPrePaidReceipt(event.modifiers+1, value);
					}
				}
				break;
			}

		case UE_PRINT_FROM_UIW_MANUAL:
			{
				// reset count
				int cNum = event.region.left/CLUSTER_SIZE;
				int bNum = event.region.left%CLUSTER_SIZE;

				RTEngine->SetNumOfCalls(cNum, bNum, 0);

                CashReq = TRUE;
                // cancel multiple prepaid. v.219
				if (g_cfg->MANUAL && g_cfg->MULTIPLE_PREPAID_CALLS)
                {
					RTEngine->SetPreValue(cNum, bNum, 0.0F);
					RTEngine->SetPrePaid (cNum, bNum, FALSE);
                }
                break;
            }
        case UE_FLUSH_ALL:
            {
                UIW_WINDOW *msgWindow = new UIW_WINDOW("MESSAGE", UI_WINDOW_OBJECT::defaultStorage);
                UI_WINDOW_OBJECT::windowManager->Center(msgWindow);
                (void *)&(*UI_WINDOW_OBJECT::windowManager + msgWindow);
                eventManager->DeviceState(E_MOUSE, DM_WAIT);
                //
				g_dbEngine->Flush();
                //
                (void *)&(*UI_WINDOW_OBJECT::windowManager - msgWindow);
                delete msgWindow;
                eventManager->DeviceState(E_MOUSE, DM_VIEW);
                break;
            }
        case UE_REBUILD_ALL:
            {
                if (UI_WINDOW_OBJECT::errorSystem->ReportError(UI_WINDOW_OBJECT::windowManager,
                        WOS_INVALID, (char *)CSTR_CRITICAL) == WOS_INVALID
                   )
                {
                    UIW_WINDOW *msgWindow = new UIW_WINDOW("MESSAGE", UI_WINDOW_OBJECT::defaultStorage);
                    UI_WINDOW_OBJECT::windowManager->Center(msgWindow);
                    (void *)&(*UI_WINDOW_OBJECT::windowManager + msgWindow);
                    eventManager->DeviceState(E_MOUSE, DM_WAIT);
                    //
					g_dbEngine->Repair();
					g_dbEngine->ExtRepair();
                    //
					LONG higher = g_dbEngine->GetHigherNumber();
                    if (higher)
						g_cfg->N_RECEIPT = higher;
					extern SUPER_APP_INFO g_superAppInfo;
#if !defined(__TEST__)
#if !defined(__NOAPPINFO__)
					if (g_superAppInfo.Attr.STPro)
                    {
						higher = g_dbEngine->ExtGetHigherNumber();
                        if (higher)
							g_cfg->E_N_RECEIPT = higher;
					}
#endif // !defined(__TEST__)
#endif // !defined(__NOAPPINFO__)
					g_cfg->Save();
					//
					(void *)&(*UI_WINDOW_OBJECT::windowManager - msgWindow);
					delete msgWindow;
					eventManager->DeviceState(E_MOUSE, DM_VIEW);
				}
				break;
			}
		case UE_ADM_REC:
			{
				// --- update in general list
				long number  = *(long *)event.data; // v.219d
				int   modifier = event.modifiers;
				if (!g_dbEngine->Update(number, modifier))
				{
					// bad record !!!
					UI_WINDOW_OBJECT::errorSystem->ReportError(UI_WINDOW_OBJECT::windowManager,
							WOS_NO_STATUS, "El recibo %ld no existe", number); // v.219d
					break;
				}
				// --- try to put into processed receipts
				DynamicReceipt dynReceipt;
				UINT numOfObjects = Receipts->GetCount();
				for (int i=0; i<numOfObjects; i++)
				{
					if (Receipts->Get(dynReceipt))
					{
						if (dynReceipt.m_r.Number == number)
						{
							dynReceipt.m_r.Stat.Paid = modifier;
							Receipts->Put(dynReceipt);
							break;
						}
						else
							Receipts->Put(dynReceipt);
					}
				}
				// not found, not even processed
				break;
            }
        case UE_NUMBER_FROM_UIW_RECEIPT:
            {
                UIW_RECEIPT *wReceipts = (UIW_RECEIPT *) event.data;
                switch (wReceipts->Option)
                {
                case UE_RNPNP:
                    // not used
                    break;
                case UE_RNPP:
                    // not used
                    break;
                case UE_RP:
                    {
						STR512 s;
						Receipt receipt;
						wReceipts->WList->Destroy();

						// begin 2.21.8 build 9
						DB_STORAGE::Iterator it(g_dbEngine->GetDBStorage());
						it.Restart(wReceipts->Number);

						long number;
						while (it)
						{
							number = it.Current();
							if (g_dbEngine->Get(receipt, number))
							{
								// just for Normal receipts
								if (receipt.Tag == Receipt::TEL && receipt.Stat.Printed)
								{
									sprintf(
										s,
										"%-8ld %-3d %-18s %4.1f     %4.1f     %2d     %12.2f",
										receipt.Number,
										receipt.BoothNumber+1,
										receipt.Phone,
										g_Milisec2Time(receipt.ElapsedTime, g_cfg->CORRECTION_TIME),
										receipt.CeilMin,
										receipt.Tariff,
										receipt.Value
									);
									*(wReceipts->WList)
										+ new UIW_STRING(1, 0,  70, s, -1, STF_NO_FLAGS, WOF_NO_FLAGS);
								}
							}

							it++;
						}

						// end 2.21.8 build 9

						wReceipts->WList->Event(UI_EVENT(S_REDISPLAY, 0));
						break;
					}
				}
				break;
            }
        case UE_BOOTH_FROM_UIW_RECEIPT:
            {
                STR512 s;
                UIW_RECEIPT *wReceipts = (UIW_RECEIPT *) event.data;
				Receipt receipt;
                switch (wReceipts->Option)
                {
				case UE_RNPNP:
					{
						int bNum = wReceipts->Booth;
						wReceipts->WList->Destroy();

						// begin 2.21.8 build 9
						DB_STORAGE::Iterator it(g_dbEngine->GetDBStorage());
						it.Restart();

						long number;
						while (it)
						{
							number = it.Current();
							if (g_dbEngine->Get(receipt, number, bNum))
							{
								if
								(
									receipt.Tag == Receipt::TEL &&
									!receipt.Stat.Printed &&
									receipt.Stat.Paid != PAID_CALL
								)
								{
									sprintf(
										s,
										"%-8ld %-3d %-18s %4.1f     %4.1f     %2d     %12.2f",
										receipt.Number,
										receipt.BoothNumber+1,
										receipt.Phone,
										g_Milisec2Time(receipt.ElapsedTime, g_cfg->CORRECTION_TIME),
										receipt.CeilMin,
										receipt.Tariff,
										receipt.Value
									);
									*(wReceipts->WList)
										+ new UIW_STRING(1, 0,  70, s, -1, STF_NO_FLAGS, WOF_NO_FLAGS);
								}
							}
							it++;
						}

						// end 2.21.8 build 9

						wReceipts->WList->Event(UI_EVENT(S_REDISPLAY, 0));
						break;
					}
				case UE_RNPP:
					{
                        int bNum = wReceipts->Booth;
						wReceipts->WList->Destroy();


						// begin 2.21.8 build 9
						DB_STORAGE::Iterator it(g_dbEngine->GetDBStorage());
						it.Restart();

						long number;
						while (it)
						{
							number = it.Current();
							if (g_dbEngine->Get(receipt, number, bNum))
							{
								if
								(
									receipt.Tag == Receipt::TEL &&
									!receipt.Stat.Printed &&
									receipt.Stat.Paid == PAID_CALL
								)
								{
									sprintf(
										s,
										"%-8ld %-3d %-18s %4.1f     %4.1f     %2d     %12.2f",
										receipt.Number,
										receipt.BoothNumber+1,
										receipt.Phone,
										g_Milisec2Time(receipt.ElapsedTime, g_cfg->CORRECTION_TIME),
										receipt.CeilMin,
										receipt.Tariff,
										receipt.Value
									);
									*(wReceipts->WList)
									+ new UIW_STRING(1, 0,  70, s, -1, STF_NO_FLAGS, WOF_NO_FLAGS);
								}
							}
						}

						// end 2.21.8 build 9

						wReceipts->WList->Event(UI_EVENT(S_REDISPLAY, 0));
                        break;
                    }
                }
            }
        case UE_PAY_FROM_UIW_RECEIPT:
            {
                // ---
                // check all items of list with the WOS_SELECTED flag,
                // change the flag, substract from list
                // and force to redisplay.
                // ---
                long number;
				DynamicReceipt dynReceipt;
				Receipt receipt;
                UI_WINDOW_OBJECT *wObject;
                UIW_RECEIPT *wReceipts = (UIW_RECEIPT *) event.data;
                UIW_STRING *wString = (UIW_STRING *) wReceipts->WList->First();
                while (wString)
                {
                    if (FlagSet(wString->woStatus, WOS_SELECTED))
                    {
                        sscanf(wString->DataGet(), "%ld", &number);
						if (g_dbEngine->Get(receipt, number))
                        {
	                        dynReceipt.m_r = receipt;
							dynReceipt.m_r.Stat.Paid = PAID_CALL;
							// begin 2.21.8
							if (!g_dbEngine->Update(dynReceipt))
							{
								if (TraceInfo::s_bTest)
								{
									UI_WINDOW_OBJECT::errorSystem->ReportError
									(
										UI_WINDOW_OBJECT::windowManager, WOS_NO_STATUS,
										"Recibo no actualizado (CTRL_EV::561)!\r\n"
									);
								}
							}
							// end 2.21.8
                            // save the item
                            wObject = wString;
                            wString = (UIW_STRING *)wString->Next();
                            // subtract from from list
                            wReceipts->WList->Subtract(wObject);
						}
                        else
                            wString = (UIW_STRING *)wString->Next();
                    }
                    else
                        wString = (UIW_STRING *)wString->Next();
                }
                //
                wReceipts->WList->Event(UI_EVENT(S_REDISPLAY, 0));
                break;
            }
        case UE_PRINT_FROM_UIW_RECEIPT:
            {
                // ---
                // idem PAY but this time we have to send to the printer.
                // ---
				DynamicReceipt dynReceipt;
				Receipt receipt;
                long number;
				UI_WINDOW_OBJECT *wObject;
                UIW_RECEIPT *wReceipts = (UIW_RECEIPT *) event.data;
                UIW_STRING *wString = (UIW_STRING *) wReceipts->WList->First();
                while (wString)
                {
                    if (FlagSet(wString->woStatus, WOS_SELECTED))
                    {
						sscanf(wString->DataGet(), "%ld", &number);
						if (g_dbEngine->Get(receipt, number))
						{
							// ---
							// print by putting in queue !!!
							// we have to ensure that the receipt never
							// goes again to the storage. set Printed to TRUE in Database
							// and FALSE to force print and not archiving in process queue
							// ---
							dynReceipt.m_r = receipt;
							dynReceipt.m_r.Stat.Printed = TRUE;
							// begin 2.21.8
							if (!g_dbEngine->Update(dynReceipt))
							{
								if (TraceInfo::s_bTest)
								{
									UI_WINDOW_OBJECT::errorSystem->ReportError
									(
										UI_WINDOW_OBJECT::windowManager, WOS_NO_STATUS,
										"Recibo no actualizado (CTRL_EV::561)!\r\n"
									);
								}
							}
							// end 2.21.8
							dynReceipt.DecTime_ = g_Milisec2Time(dynReceipt.m_r.ElapsedTime,
								g_cfg->CORRECTION_TIME),
							dynReceipt.m_r.Stat.Printed  = FALSE;
							dynReceipt.m_r.Stat.Archived = TRUE;
							dynReceipt.m_r.Stat.Manual   = FALSE; // automatic !!!
							dynReceipt.m_r.Stat.Cooked   = TRUE;
							dynReceipt.PreValue_       = 0.0F;
							dynReceipt.MoneyBack_      = -1; // to avoid the printing
							dynReceipt.Attr_.HeaderOn  = TRUE;
							dynReceipt.Attr_.FooterOn  = TRUE;
							dynReceipt.Attr_.SummaryOn = FALSE;
							dynReceipt.Attr_.Storable  = FALSE;
							dynReceipt.Attr_.Countable = FALSE;
							dynReceipt.Attr_.Printable = TRUE;
							Receipts->Put(dynReceipt);
							//
                            // save the item for possible deletion !!!
                            wObject  = wString;
                            wString = (UIW_STRING *)wString->Next();
							// substract from from list if it wasn't printed
							// else de-select !!!
                            if (wReceipts->Option != UE_RP)
                                wReceipts->WList->Subtract(wObject);
                            else
                                wObject->woStatus &= ~WOS_SELECTED;
                        }
                        else
                            wString=(UIW_STRING *)wString->Next();
                    }
                    else
                        wString=(UIW_STRING *)wString->Next();
				}

                //
                wReceipts->WList->Event(UI_EVENT(S_REDISPLAY, 0));
                break;
            }
        case UE_CANCEL_FROM_UIW_RECEIPT:
            {
                //
				break;
            }
        case UE_PRINT_FROM_UIW_SP_SERV:
            {
				Receipts->Put(*(DynamicReceipt *)event.data);
                break;
            }
        case UE_PRINT_FROM_DB_VIEW:
            {
				DynamicReceipt dynReceipt = *(DynamicReceipt *)event.data;
				// ---
				// print by putting in queue !!!
				// we have to ensure that the receipt never
				// goes again to the storage. set Printed to TRUE in Database
				// and FALSE to force print and not archiving in process queue
				// ---
				dynReceipt.m_r.Stat.Printed = TRUE;
				// dynReceipt.DecTime_ = g_Milisec2Time(dynReceipt.m_r.ElapsedTime, g_cfg->CORRECTION_TIME),
				dynReceipt.m_r.Stat.Printed  = FALSE;
				dynReceipt.m_r.Stat.Archived = TRUE;
				dynReceipt.m_r.Stat.Manual   = FALSE; // automatic !!!
				dynReceipt.PreValue_       = 0.0F;
				dynReceipt.MoneyBack_      = -1; // to avoid the printing
				dynReceipt.Attr_.HeaderOn  = TRUE;
				dynReceipt.Attr_.FooterOn  = TRUE;
				dynReceipt.Attr_.SummaryOn = FALSE;
				dynReceipt.Attr_.Storable  = FALSE;
				dynReceipt.Attr_.Countable = FALSE;
				dynReceipt.Attr_.Printable = TRUE;
				Receipts->Put(dynReceipt);
				break;
			}
		case UE_VIEW_FROM_UIW_ACCUM:
		case UE_VIEW_FROM_UIW_SACCUM:
			{
				UI_WINDOW_OBJECT *window = (UI_WINDOW_OBJECT *)event.data;
				displayStatistics(window, event.modifiers, window->userFlags);
				break;
			}
		case UE_PRINT_FROM_UIW_ACCUM:
			{
				// Operator
				UIW_ACCUM  *wStatistics = (UIW_ACCUM *) event.data;
				UIW_STRING *wString;
				wString = (UIW_STRING *) wStatistics->Get("W_NAME");
				if (strcmp(wString->DataGet(), g_cfg->OP_NAME))
				{
					strcpy(g_cfg->OP_NAME, wString->DataGet());
					g_cfg->Save();
				}
				printStatistics(DB_STATISTICS::TURN, wStatistics->userFlags);
				//
				if (event.modifiers)
				{ // delete accumm ?
					g_dbEngine->Archive();
					// archive log. v.219.3
					Log log(Log::OUT|Log::CREATE);
					log.archive();
					// first event
					int date, time;

#if !defined(__TEST__)
					g_STM2->get(STM2::LOGINDATE, &date);
					g_STM2->get(STM2::LOGINTIME, &time);
					log.put(date, time, Log::NORMALSTART);
#else
					log.put(Log::NORMALSTART);
#endif
					// v.219.3. go to next turn
					g_cfg->nextTurn();
					// v.2.20.8
					g_cfg->Save();
				}
				break;
			}
		case UE_PRINT_FROM_UIW_SACCUM:
			{
				UI_WINDOW_OBJECT *window = (UI_WINDOW_OBJECT *)event.data;
				printStatistics((int)event.modifiers, window->userFlags);
                break;
            }
        case UE_UPDATE_HLP_BAR:
            UI_WINDOW_OBJECT *HelpObject = (UI_WINDOW_OBJECT *) event.data;
            View->WStatBar->Update(HelpObject);
            break;
        case UE_LOADPR_DLLS:
            {
                prnFormatter->unload();
				prnFormatter->load(g_cfg->FORM);
                break;
            }
        case UE_REFRESH_STBAR:
            View->WStatBar->Event(UI_EVENT(S_SIZE, 0));
            break;
        case UE_DISPLAY:
            { // v2.18 booth display
				if (g_cfg->DISPLAY_ENABLE)
                {
                    if (boothDisplay)
                    {
                        if (!boothDisplay->install(
									g_cfg->DISPLAY_DEFAULT_MESSAGE,
									g_cfg->DISPLAY_COM,
									g_cfg->DISPLAY_BAUDS
                                )
                           )
                        {
                            delete boothDisplay;
                            boothDisplay = NULL;
                        }
                    }
                }
                break;
            }
        case UE_UPDATE_TOTAL_DISPLAY:
            {
				if (g_cfg->DISPLAY_ENABLE)
                {
                    if (boothDisplay)
                    {
                        static BoothDisplay::Info *info;
                        info = (BoothDisplay::Info *)event.data;
                        boothDisplay->showTotalCash(*info);
                    }
                }
                break;
            }
        }
        ccode = state;
        break;
        //
    }
    // Return the control code.
    return (ccode);
}

void CONTROLLER::displayStatistics(void *window, WORD type, BOOL fromTurn)
{
    UIW_WINDOW *wStatistics;
    if (type == DB_STATISTICS::TURN)
        wStatistics = (UIW_ACCUM *) window;
    else
        wStatistics = (UIW_SACCUM *) window;
    //
    UIW_STRING  *wString;
    UIW_BIGNUM  *wBignum;
    UIW_INTEGER *wInteger;
    UIW_DATE    *wDate;
    UIW_TIME    *wTime;
    //
    DS_ENTRY *entry;
    DS_CELLULARENTRY *cellularEntry;
    if (fromTurn)
    {
		entry = (*g_dbEngine)[type];
		cellularEntry = g_dbEngine->GetCellularEntry(type);
    }
    else
    {
		entry = g_dbEngine->GetArcStatistics(type);
		cellularEntry = g_dbEngine->GetArcCellularEntry(type);
    }
    if (type == DB_STATISTICS::TURN)
    {
        // Operator
        wString = (UIW_STRING *) wStatistics->Get("W_NAME");
        wString->DataSet(g_cfg->OP_NAME);
    }
    // From
    if (entry->From.Time)
    {
        wTime = (UIW_TIME *) wStatistics->Get("W_FT");
        wTime->DataSet(&UI_TIME(entry->From.Time));
    }
    if (entry->From.Date)
    {
        wDate = (UIW_DATE *) wStatistics->Get("W_FD");
        wDate->DataSet(&UI_DATE(entry->From.Date));
    }
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_FR");
    wBignum->DataSet(&UI_BIGNUM(entry->From.Number));
    // To
    if (entry->To.Time)
    {
        wTime = (UIW_TIME *) wStatistics->Get("W_TT");
        wTime->DataSet(&UI_TIME(entry->To.Time));
    }
    if (entry->To.Date)
    {
        wDate = (UIW_DATE *) wStatistics->Get("W_TD");
        wDate->DataSet(&UI_DATE(entry->To.Date));
    }
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_TR");
    wBignum->DataSet(&UI_BIGNUM(entry->To.Number));
    // Not paid and toll free
    wInteger = (UIW_INTEGER *) wStatistics->Get("W_NPR");
    wInteger->DataSet((int *)&entry->NotPaid.Receipts);
    // v.211	wBignum = (UIW_BIGNUM *) wStatistics->Get("W_NPM");
    // v.211	wBignum->DataSet(&UI_BIGNUM(entry->NotPaid.PaidMin));
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_NPV");
    wBignum->DataSet(&UI_BIGNUM(entry->NotPaid.Value));
    //
    wInteger = (UIW_INTEGER *) wStatistics->Get("W_TFR");
    wInteger->DataSet((int *)&entry->TollFree.Receipts);
    // v.211	wBignum = (UIW_BIGNUM *) wStatistics->Get("W_TFM");
    // v.211	wBignum->DataSet(&UI_BIGNUM(entry->TollFree.PaidMin));
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_TFV");
    wBignum->DataSet(&UI_BIGNUM(entry->TollFree.Value));
    //
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_NP_TOTAL");
    wBignum->DataSet(&UI_BIGNUM(entry->Total.NotPaid));
    // Tel recs: Nal and inter
    wInteger = (UIW_INTEGER *) wStatistics->Get("W_NR");
    WORD receipts = entry->Tel.Nal.Receipts+cellularEntry->Tel.Receipts;
    wInteger->DataSet((int *)&receipts);
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_NMT");
    double talkMin = entry->Tel.Nal.TalkMin + cellularEntry->Tel.TalkMin;
    wBignum->DataSet(&UI_BIGNUM(talkMin));
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_NMP");
    double paidMin = entry->Tel.Nal.PaidMin + cellularEntry->Tel.PaidMin;
    wBignum->DataSet(&UI_BIGNUM(paidMin));
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_NV");
    double value = entry->Tel.Nal.Value + cellularEntry->Tel.Value;
    wBignum->DataSet(&UI_BIGNUM(value));
    //
    wInteger = (UIW_INTEGER *) wStatistics->Get("W_IR");
    wInteger->DataSet((int *)&entry->Tel.Inter.Receipts);
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_IMT");
    wBignum->DataSet(&UI_BIGNUM(entry->Tel.Inter.TalkMin));
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_IMP");
    wBignum->DataSet(&UI_BIGNUM(entry->Tel.Inter.PaidMin));
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_IV");
    wBignum->DataSet(&UI_BIGNUM(entry->Tel.Inter.Value));
    //
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_N_SUB");
    wBignum->DataSet(&UI_BIGNUM(entry->Total.Tel-entry->Tax.Tel));
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_N_TAX");
    wBignum->DataSet(&UI_BIGNUM(entry->Tax.Tel));
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_N_TOTAL");
    wBignum->DataSet(&UI_BIGNUM(entry->Total.Tel));
    // Special recs: Nal and inter
    wInteger = (UIW_INTEGER *) wStatistics->Get("W_SNTR");
    receipts = entry->SpecialTel.Nal.Receipts+cellularEntry->SpecialTel.Receipts;
    wInteger->DataSet((int *)&receipts);
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_SNTV");
    value = entry->SpecialTel.Nal.Value + cellularEntry->SpecialTel.Value;
    wBignum->DataSet(&UI_BIGNUM(value));
    wInteger = (UIW_INTEGER *) wStatistics->Get("W_SNXR");
	wInteger->DataSet((int *)&entry->Internet.Nal.Receipts);
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_SNXV");
	wBignum->DataSet(&UI_BIGNUM(entry->Internet.Nal.Value));
    wInteger = (UIW_INTEGER *) wStatistics->Get("W_SNFR");
    wInteger->DataSet((int *)&entry->Fax.Nal.Receipts);
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_SNFV");
    wBignum->DataSet(&UI_BIGNUM(entry->Fax.Nal.Value));
    //
    wInteger = (UIW_INTEGER *) wStatistics->Get("W_SNMR");
    int mCards = 0;
    for (int i=0; i<MAX_MAGNETIC_CARDS; i++)
        mCards += entry->Cards.Cards[i];
    wInteger->DataSet(&mCards);
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_SNMV");
    wBignum->DataSet(&UI_BIGNUM(entry->Cards.Value));
    wInteger = (UIW_INTEGER *) wStatistics->Get("W_SNOR");
    wInteger->DataSet((int *)&entry->Other.Receipts);
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_SNOV");
    wBignum->DataSet(&UI_BIGNUM(entry->Other.Value));
    //
    wInteger = (UIW_INTEGER *) wStatistics->Get("W_SITR");
    wInteger->DataSet((int *)&entry->SpecialTel.Inter.Receipts);
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_SITV");
    wBignum->DataSet(&UI_BIGNUM(entry->SpecialTel.Inter.Value));
	wInteger = (UIW_INTEGER *) wStatistics->Get("W_SIFR");
    wInteger->DataSet((int *)&entry->Fax.Inter.Receipts);
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_SIFV");
    wBignum->DataSet(&UI_BIGNUM(entry->Fax.Inter.Value));
    //
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_S_SUB");
    wBignum->DataSet(&UI_BIGNUM(entry->Total.Special-entry->Tax.Special));
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_S_TAX");
    wBignum->DataSet(&UI_BIGNUM(entry->Tax.Special));
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_S_TOTAL");
    wBignum->DataSet(&UI_BIGNUM(entry->Total.Special));
    // errores
    wInteger = (UIW_INTEGER *) wStatistics->Get("W_DERR");
    wInteger->DataSet((int *)&entry->DialErrors);
    wInteger = (UIW_INTEGER *) wStatistics->Get("W_CERR");
    wInteger->DataSet((int *)&entry->ComErrors);
    // general total
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_SUB");
    wBignum->DataSet(&UI_BIGNUM(entry->Total.General-entry->Tax.General));
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_TAX");
    wBignum->DataSet(&UI_BIGNUM(entry->Tax.General));
    wBignum = (UIW_BIGNUM *) wStatistics->Get("W_TOTAL");
    wBignum->DataSet(&UI_BIGNUM(entry->Total.General));
}
