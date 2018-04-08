using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;

public enum DieMessageType : byte
{
    None = 0,
    State,
    Telemetry,
    BulkSetup,
    BulkSetupAck,
    BulkData,
    BulkDataAck,
    TransferAnimSet,
    TransferAnimSetAck,
    TransferAnimReadyForNextAnim,

    PlayAnim,
    PrintName,
    RequestAnimSet,
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
                case DieMessageType.State:
                    ret = FromByteArray<DieMessageState>(data);
                    break;
                case DieMessageType.Telemetry:
                    ret = FromByteArray<DieMessageAcc>(data);
                    break;
                case DieMessageType.BulkSetup:
                    ret = FromByteArray<DieMessageBulkSetup>(data);
                    break;
                case DieMessageType.BulkData:
                    ret = FromByteArray<DieMessageBulkData>(data);
                    break;
                case DieMessageType.BulkSetupAck:
                    ret = FromByteArray<DieMessageBulkSetupAck>(data);
                    break;
                case DieMessageType.BulkDataAck:
                    ret = FromByteArray<DieMessageBulkDataAck>(data);
                    break;
                case DieMessageType.TransferAnimSet:
                    ret = FromByteArray<DieMessageTransferAnimSet>(data);
                    break;
                case DieMessageType.TransferAnimSetAck:
                    ret = FromByteArray<DieMessageTransferAnimSetAck>(data);
                    break;
                case DieMessageType.RequestAnimSet:
                    ret = FromByteArray<DieMessageRequestAnimSet>(data);
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

[StructLayout(LayoutKind.Sequential)]
public class DieMessageState
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.State;
    public byte face;
}

[StructLayout(LayoutKind.Sequential)]
public class DieMessageAcc
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.Telemetry;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
    public AccelFrame[] data;
}

[StructLayout(LayoutKind.Sequential)]
public class DieMessageBulkSetup
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.BulkSetup;
    public short size;
}

[StructLayout(LayoutKind.Sequential)]
public class DieMessageBulkSetupAck
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.BulkSetupAck;
}

[StructLayout(LayoutKind.Sequential)]
public class DieMessageBulkData
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.BulkData;
    public byte size;
    public short offset;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
    public byte[] data;
}

[StructLayout(LayoutKind.Sequential)]
public class DieMessageBulkDataAck
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.BulkDataAck;
}

[StructLayout(LayoutKind.Sequential)]
public class DieMessageTransferAnimSet
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.TransferAnimSet;
    public byte count;
    public short totalAnimationByteSize;
}

[StructLayout(LayoutKind.Sequential)]
public class DieMessageTransferAnimSetAck
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.TransferAnimSetAck;
}

[StructLayout(LayoutKind.Sequential)]
public class DieMessageTransferAnimReadyForNextAnim
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.TransferAnimReadyForNextAnim;
}

[StructLayout(LayoutKind.Sequential)]
public class DieMessageRequestAnimSet
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.RequestAnimSet;
}

