using System;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using UnityEngine;

public class UnityNeonChatClient : MonoBehaviour
{
    /// <summary>
    /// The client's username.
    /// </summary>
    public string clientUserName;
    
    /// <summary>
    /// The message text to be set from your chat UI.
    /// </summary>
    public string messageText;
    
    /// <summary>
    /// The buffer for indirect communication between the main thread and the native plugin.
    /// </summary>
    private static string _buffer;
    
    /// <summary>
    /// Used to update the buffer in the main thread.
    /// </summary>
    private static bool update;
    
    /// <summary>
    /// Assign your own action to this delegate to receive messages from the server.
    /// </summary>
    public static Action<string> onMessageReceivedAction;

    private void Update()
    {
        if (!update || string.IsNullOrEmpty(_buffer)) return;
        update = false;
        onMessageReceivedAction?.Invoke(_buffer);
    }

    delegate void OnNewMessageReceived(string userName, string chatMessage);
    
    [DllImport("NeonChat")]
    public static extern void ConnectToNeonChat();
    
    [DllImport("NeonChat")]
    public static extern void DisconnectFromNeonChat();

    [DllImport("NeonChat")]
    public static extern void SendNeonChat(string userName, string chatMessage);
    

    [DllImport("NeonChat")]
    static extern void RegisterOnNewMessageReceivedCallback(OnNewMessageReceived callback);

    static void OnMessageReceived(string userName, string chatMessage)
    {
        _buffer += $"\n[{userName}]: {chatMessage}";
        update = true;
    }
    
    /// <summary>
    /// Call send message to send the current message to the server. Usually set through a UI button, or called by a key press.
    /// </summary>
    public void SendMsg()
    {
        try
        {
            SendNeonChat(clientUserName, messageText);
        }
        catch (Exception e)
        {
            Debug.Log("Error sending message: " + e.Message);
        }
    }
    
    public void OnEnable()
    {
        Debug.Log("Connecting to NeonChat...");
        Task.Run(() => 
        {
            ConnectToNeonChat();
            RegisterOnNewMessageReceivedCallback(OnMessageReceived);
            onMessageReceivedAction += Debug.Log;
        }).ConfigureAwait(false);

    }
    
    public void OnDisable()
    {
        Debug.Log("Disconnecting from NeonChat...");
        onMessageReceivedAction -= Debug.Log;
        Task.Run(DisconnectFromNeonChat);
    }
}
