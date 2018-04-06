using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;

public enum DieMessageType : byte
{
    None = 0,
    Face,
    Telemetry
}

public interface DieMessage
{
    DieMessageType type { get; set; }
}

public static class DieMessages
{
    public static DieMessage FromByteArray(byte[] data)
    {
        DieMessage ret = null;
        if (data.Length > 0)
        {
            DieMessageType type = (DieMessageType)data[0];
            switch (type)
            {
                case DieMessageType.Face:
                    ret = FromByteArray<DieMessageFace>(data);
                    break;
                case DieMessageType.Telemetry:
                    ret = FromByteArray<DieMessageAcc>(data);
                    break;
                default:
                    break;
            }
        }
        return ret;
    }

    static DieMessage FromByteArray<T>(byte[] data)
        where T : DieMessage
    {
        int size = Marshal.SizeOf(typeof(T));
        System.IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.Copy(data, 0, ptr, size);
        var retMessage = (T)Marshal.PtrToStructure(ptr, typeof(T));
        Marshal.FreeHGlobal(ptr);
        return retMessage;
    }

    // For virtual dice!
    public static byte[] ToByteArray<T>(T message)
        where T : DieMessage
    {
        int size = Marshal.SizeOf(typeof(T));
        System.IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.StructureToPtr(message, ptr, false);
        byte[] ret = new byte[size];
        Marshal.Copy(ptr, ret, 0, size);
        Marshal.FreeHGlobal(ptr);
        return ret;
    }
}

public struct DieMessageFace
    : DieMessage
{
    public DieMessageType type { get; set; }
    public byte face;
}

public struct DieMessageAcc
    : DieMessage
{
    public DieMessageType type { get; set; }

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
    public AccelFrame[] data;
}


public enum CommandMessageType : byte
{
    None = 0,
    PlayAnim,
}

public interface CommandMessage
{
    CommandMessageType type { get; set; }
}

public static class CommandMessages
{
    public static byte[] ToByteArray(CommandMessage msg)
    {
        int size = Marshal.SizeOf(msg.GetType());
        System.IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.StructureToPtr(msg, ptr, false);
        byte[] ret = new byte[size];
        Marshal.Copy(ptr, ret, 0, size);
        Marshal.FreeHGlobal(ptr);
        return ret;
    }

    // For virtual dice!
    public static CommandMessage FromByteArray<T>(byte[] data)
        where T : CommandMessage
    {
        int size = Marshal.SizeOf(typeof(T));
        System.IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.Copy(data, 0, ptr, size);
        var retMessage = (T)Marshal.PtrToStructure(ptr, typeof(T));
        Marshal.FreeHGlobal(ptr);
        return retMessage;
    }
}

public struct CommandMessageAnim
    : CommandMessage
{
    public CommandMessageType type { get; set; }
    public byte anim;
}



