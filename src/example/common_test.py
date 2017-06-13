import sys
import subprocess

TEST_SQL_QUERIES = [
    # 1
    "SELECT financial.quarterly_profits,financial.quarterly_revenues,all_employees.id,\
            all_employees.name,all_employees.active,all_employees.active_direct_reports,\
            all_employees.enumx, all_employees.enumy,founded,board_of_directors\
     FROM Example1.Company",

    # 2
    "SELECT all_employees.id, all_employees.name\
     FROM Example1.Company\
     WHERE all_employees.id IS NOT NULL AND\
           ((all_employees.name='def' AND all_employees.active=true) OR\
              all_employees.name='abc') AND\
           financial.quarterly_profits>0",

    # 3
    "SELECT all_employees.id, financial.quarterly_revenues/all_employees.active_direct_reports\
     FROM Example1.Company\
     WHERE all_employees.active_direct_reports > 0 AND\
           financial.quarterly_revenues*2 > 1",

    # 4
    "SELECT financial.quarterly_profits,all_employees.name,all_employees.active\
     FROM Example1.Company\
     ORDER BY all_employees.active,financial.quarterly_profits DESC,all_employees.id ASC",

    # 5
    "SELECT STR(all_employees.id) + ': ' + all_employees.name + ' (' + STR(all_employees.active) + ')' AS employee,\
            STR(all_employees.id) + ' ' + employee + ' ' + STR(founded) AS employee2\
     FROM Example1.Company\
     WHERE employee2 LIKE '.*true.*'",

    # 6
    "SELECT all_employees.name, all_employees.active_direct_reports\
     FROM Example1.Company\
     WHERE all_employees.active_direct_reports IS NOT NULL",

    # 7
    "SELECT EmployeeStr(all_employees.name, all_employees.id), STR(all_employees.id), STR(all_employees)\
     FROM Example1.Company",

    # 8
    "SELECT *, all_employees.* AS emp\
     FROM Example1.Company",
]

if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.stderr.write('Invalid num arguments, expected atleast 2, testnum and sql query\n')
        sys.exit(1)

    testnum = int(sys.argv[1])
    if (testnum < 1) or (testnum > len(TEST_SQL_QUERIES)):
        sys.stderr.write('Invalid testnum ' + str(testnum) + ' valid range: [1,' +
                         str(len(TEST_SQL_QUERIES)) + ']')
        sys.exit(1)

    cmake_prefix_paths = []
    if len(sys.argv) >= 3:
        cmake_prefix_paths = sys.argv[2].split(',')

    SOURCE_DIR = '../../src'
    proto_file = SOURCE_DIR + '/example/example1.json'
    golden_out = 'golden' + str(testnum) + '.out'
    actual_out = 'pb_actual' + str(testnum) + '.out'

    cmd_parts = ['../main/RunQuery']
    cmd_parts.append('--codeGenDir=' + SOURCE_DIR + '/example')
    cmd_parts.append('--codeGenPrefix=pb_generated_query' + str(testnum))
    cmd_parts.append('--codeCompileDir=.')
    cmd_parts.append('--cppProtoHeader=example1.pb.h')
    cmd_parts.append('--cppProtoLib=./libExampleProtos.so')
    cmd_parts.append('--cppExtraIncludes=example1_utils.h')
    cppExtraIncludeDirs = []
    cppExtraLinkLibDirs = []
    if len(cmake_prefix_paths) > 0:
        cppExtraIncludeDirs.extend([p + '/include' for p in cmake_prefix_paths])
        cppExtraLinkLibDirs.extend([p + '/lib' for p in cmake_prefix_paths])
    if len(cppExtraIncludeDirs) > 0:
        cmd_parts.append('--cppExtraIncludeDirs=' + ','.join(cppExtraIncludeDirs))
    if len(cppExtraLinkLibDirs) > 0:
        cmd_parts.append('--cppExtraLinkLibDirs=' + ','.join(cppExtraLinkLibDirs))
    cmd_parts.append(TEST_SQL_QUERIES[testnum-1])
    cmd_parts.append(proto_file)

    # subprocess.check* will raise error if command fails
    sys.stderr.write('RUNNING ' + ' '.join(cmd_parts) + '\n')
    with open(actual_out, 'w') as acutal_hndl:
        subprocess.check_call(cmd_parts, stdout=acutal_hndl)    

    # diff doesn't return non zero exit if files differ, so do it manually
    cmd_parts = ['diff', '--brief', golden_out, actual_out]
    sys.stderr.write('RUNNING ' + ' '.join(cmd_parts) + '\n')
    diff_res = subprocess.check_output(cmd_parts)
    if len(diff_res) > 0:
        sys.stderr.write(diff_res)
        exit(1)
    sys.stderr.write('Success\n')
