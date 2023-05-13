// CS0079: The event `Foo.BaseFoo.Changed' can only appear on the left hand side of `+=' or `-=' operator
// Line: 16

using System;

namespace Foo
{
	public delegate void VoidHandler ();

	public class BaseFoo
	{
		public extern event VoidHandler Changed;

		public string Name {
			set {
				Changed ();
			}
		}
	}
}
