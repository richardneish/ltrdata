#if !defined(_INC_WBASE_HPP_) && defined(__cplusplus)
#define _INC_WBASE_HPP_

/**
   Base class for all classes that embeds handles to different system objects,
   such as kernel objects, user objects, SCM objects, registry keys etc, for
   short, any object that can me duplicated using DuplicateHandle().

   This class has no public constructors. That is because there cannot be any
   public desctructor since the objects may contain handle to any object and
   it is not possible to know how a handle should be closed if all we know is
   that it is a handle to something.
*/
class WObjectHandle
{
protected:
  /// The handle to the embedded kernel object.
  HANDLE hObject;

  explicit WObjectHandle(HANDLE handle = INVALID_HANDLE_VALUE)
    : hObject(handle)
  {
  }

public:
  /// Returns the handle to the embedded kernel object.
  HANDLE Handle() const
  {
    return hObject;
  }
  /// Returns a new handle duplicated from the embedded. 
  HANDLE NewHandle(BOOL bInheritHandle = FALSE,
		   DWORD dwOptions = DUPLICATE_SAME_ACCESS,
		   DWORD dwDesiredAccess = 0)
    const
  {
    if (!IsValidHandle(hObject))
      return INVALID_HANDLE_VALUE;

    HANDLE hNew;
    if (!DuplicateHandle(GetCurrentProcess(), hObject, GetCurrentProcess(),
			 &hNew, dwDesiredAccess, bInheritHandle, dwOptions))
      return INVALID_HANDLE_VALUE;

    return hNew;
  }

  bool ReCreate(BOOL bInheritHandle = FALSE,
		DWORD dwOptions = DUPLICATE_SAME_ACCESS,
		DWORD dwDesiredAccess = 0)
  {
    if (!IsValidHandle(hObject))
      return false;

    if (!DuplicateHandle(GetCurrentProcess(), hObject, GetCurrentProcess(),
			 &hObject, dwDesiredAccess | DUPLICATE_CLOSE_SOURCE,
			 bInheritHandle, dwOptions))
      {
	hObject = INVALID_HANDLE_VALUE;
	return false;
      }

    return true;
  }

  /// Returns true if embedded handle is invalid.
  bool operator!() const
  {
    return (hObject == INVALID_HANDLE_VALUE) | (hObject == NULL);
  }

  /// Returns true if embedded handle is valid.
  operator bool() const
  {
    return IsValidHandle(hObject);
  }

  /// Returns true if this object's handle is valid.
  bool IsValidHandle() const
  {
    return (hObject != INVALID_HANDLE_VALUE) & (hObject != NULL);
  }

  /// Returns true if given handle is valid.
  static bool IsValidHandle(HANDLE handle)
  {
    return (handle != INVALID_HANDLE_VALUE) & (handle != NULL);
  }
};

/**
   Base class for all kernel object wrapping classes.
   Uses CloseHandle() on the embedded handle when object is destroyed.
*/
class WKernelObject : public WObjectHandle
{
public:
  /// Init with existing handle.
  explicit WKernelObject(HANDLE handle = INVALID_HANDLE_VALUE)
    : WObjectHandle(handle)
  {
  }

  /// Init with handle from other object.
  explicit WKernelObject(const WKernelObject &obj)
    : WObjectHandle(obj.Handle())
  {
  }

  /// Closes existing object and init with handle from other object.
  WKernelObject& operator=(const WKernelObject &obj)
  {
    Close();
    hObject = obj.Handle();
    return *this;
  }

  /// Closes existing object and init with duplicate of handle.
  WKernelObject& operator=(HANDLE handle)
  {
    Close();

    hObject = handle;

    return *this;
  }

  /// Closes the handle.
  ~WKernelObject()
  {
    Close();
  }

  bool Duplicate(HANDLE hExisting,
		 BOOL bInheritHandle = FALSE,
		 DWORD dwOptions = DUPLICATE_SAME_ACCESS,
		 DWORD dwDesiredAccess = 0)
  {

    Close();

    if (!DuplicateHandle(GetCurrentProcess(), hExisting, GetCurrentProcess(),
			 &hObject, dwDesiredAccess, bInheritHandle, dwOptions))
      {
	hObject = INVALID_HANDLE_VALUE;
	return false;
      }

    return true;
  }

  /// Closes the embedded handle if it is valid.
  bool Close()
  {
    if (!IsValidHandle(hObject))
      return false;

    if (CloseHandle(hObject))
      {
	hObject = INVALID_HANDLE_VALUE;
	return true;
      }
    else
      return false;
  }

};
#endif // _INC_WBASE_HPP_

