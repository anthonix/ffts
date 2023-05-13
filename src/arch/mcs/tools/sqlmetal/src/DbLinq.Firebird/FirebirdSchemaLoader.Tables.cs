#region MIT license
// 
// MIT license
//
// Copyright (c) 2007-2008 Jiri Moudry, Pascal Craponne
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
#endregion
using System.Collections.Generic;
using System.Data;
using DbLinq.Util;
using DbLinq.Vendor;
using DbLinq.Vendor.Implementation;

namespace DbLinq.Firebird
{
#if !MONO_STRICT
    public
#endif
    partial class FirebirdSchemaLoader
    {
        public override IList<IDataName> ReadTables(IDbConnection connectionString, string databaseName)
        {
            // note: the ReadDataNameAndSchema relies on information order
            const string sql = @"
select RDB$RELATION_NAME ""TableName""
        , 'Foo' ""TableSchema""
    from RDB$RELATIONS
    where RDB$SYSTEM_FLAG = 0
    order by RDB$RELATION_NAME
";
            return DataCommand.Find<IDataName>(connectionString, sql, "@db", databaseName, ReadDataNameAndSchema)
                .ConvertAll(delegate(IDataName dn) {
                    dn.Name = dn.Name.Trim();
                    return dn;
                });
        }
    }
}
