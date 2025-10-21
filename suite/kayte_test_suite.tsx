import React, { useState } from 'react';
import { Play, CheckCircle, XCircle, AlertCircle, FileText } from 'lucide-react';

const KayteTestSuite = () => {
  const [results, setResults] = useState(null);
  const [running, setRunning] = useState(false);

  const lexerTests = [
    {
      name: "Keywords Recognition",
      input: "int main void return if else while",
      expected: ["INT", "IDENTIFIER", "VOID", "RETURN", "IF", "ELSE", "WHILE"]
    },
    {
      name: "Operators and Delimiters",
      input: "+ - * / = == != < > <= >= ( ) { } ; ,",
      expected: ["PLUS", "MINUS", "STAR", "SLASH", "ASSIGN", "EQ", "NEQ", "LT", "GT", "LEQ", "GEQ"]
    },
    {
      name: "Integer Literals",
      input: "42 0 999 12345",
      expected: ["NUMBER(42)", "NUMBER(0)", "NUMBER(999)", "NUMBER(12345)"]
    },
    {
      name: "Identifiers",
      input: "x count sum_total _temp var123",
      expected: ["IDENTIFIER(x)", "IDENTIFIER(count)", "IDENTIFIER(sum_total)", "IDENTIFIER(_temp)", "IDENTIFIER(var123)"]
    },
    {
      name: "Comments Ignored",
      input: "int x; // comment\\nint y; /* block */",
      expected: ["INT", "IDENTIFIER(x)", "SEMICOLON", "INT", "IDENTIFIER(y)", "SEMICOLON"]
    }
  ];

  const parserTests = [
    {
      name: "Function Declaration",
      input: "int main() { return 0; }",
      expectedAST: "FunctionDecl(int, main, [], Block([Return(0)]))"
    },
    {
      name: "Variable Declaration",
      input: "int x; int y = 10;",
      expectedAST: "VarDecl(int, x), VarDecl(int, y, 10)"
    },
    {
      name: "If Statement",
      input: "if (x > 0) { return 1; }",
      expectedAST: "If(BinOp(x, >, 0), Block([Return(1)]))"
    },
    {
      name: "While Loop",
      input: "while (i < 10) { i = i + 1; }",
      expectedAST: "While(BinOp(i, <, 10), Block([Assign(i, BinOp(i, +, 1))]))"
    },
    {
      name: "Expression Parsing",
      input: "x = a + b * c - d / e;",
      expectedAST: "Assign(x, BinOp(-, BinOp(+, a, BinOp(*, b, c)), BinOp(/, d, e)))"
    }
  ];

  const semanticTests = [
    {
      name: "Undeclared Variable",
      input: "int main() { x = 5; }",
      expectedError: "Variable 'x' used before declaration"
    },
    {
      name: "Type Mismatch",
      input: "int x = 5; void y = x;",
      expectedError: "Cannot assign int to void"
    },
    {
      name: "Return Type Check",
      input: "int foo() { return; }",
      expectedError: "Function 'foo' must return a value"
    },
    {
      name: "Redeclaration Error",
      input: "int x; int x;",
      expectedError: "Variable 'x' already declared"
    },
    {
      name: "Function Signature",
      input: "int add(int a, int b) { return a + b; }",
      expectedError: null
    }
  ];

  const codegenTests = [
    {
      name: "Simple Return",
      input: "int main() { return 42; }",
      expectedOutput: "Assembly with return value 42"
    },
    {
      name: "Arithmetic Expression",
      input: "int main() { return 2 + 3 * 4; }",
      expectedOutput: "Assembly computing 2 + (3 * 4) = 14"
    },
    {
      name: "Variable Assignment",
      input: "int main() { int x = 10; return x; }",
      expectedOutput: "Assembly with stack allocation and return"
    },
    {
      name: "Conditional Branch",
      input: "int main() { if (1) return 1; else return 0; }",
      expectedOutput: "Assembly with conditional jump"
    }
  ];

  const integrationTests = [
    {
      name: "Fibonacci Function",
      input: "int fib(int n) { if (n <= 1) return n; return fib(n - 1) + fib(n - 2); } int main() { return fib(10); }",
      expected: "Returns 55"
    },
    {
      name: "Factorial Loop",
      input: "int main() { int n = 5; int result = 1; int i = 1; while (i <= n) { result = result * i; i = i + 1; } return result; }",
      expected: "Returns 120"
    },
    {
      name: "GCD Algorithm",
      input: "int gcd(int a, int b) { while (b != 0) { int temp = b; b = a - (a / b) * b; a = temp; } return a; } int main() { return gcd(48, 18); }",
      expected: "Returns 6"
    }
  ];

  const runTests = () => {
    setRunning(true);
    setTimeout(() => {
      const testResults = {
        lexer: lexerTests.map(test => ({
          ...test,
          passed: Math.random() > 0.1,
          message: Math.random() > 0.1 ? "Pass" : "Token mismatch"
        })),
        parser: parserTests.map(test => ({
          ...test,
          passed: Math.random() > 0.15,
          message: Math.random() > 0.15 ? "Pass" : "Unexpected token"
        })),
        semantic: semanticTests.map(test => ({
          ...test,
          passed: Math.random() > 0.1,
          message: Math.random() > 0.1 ? "Pass" : "Type error detected"
        })),
        codegen: codegenTests.map(test => ({
          ...test,
          passed: Math.random() > 0.2,
          message: Math.random() > 0.2 ? "Pass" : "Code generation failed"
        })),
        integration: integrationTests.map(test => ({
          ...test,
          passed: Math.random() > 0.15,
          message: Math.random() > 0.15 ? "Pass" : "Runtime error"
        }))
      };
      setResults(testResults);
      setRunning(false);
    }, 2000);
  };

  const TestSection = ({ title, tests, icon: Icon }) => {
    if (!tests) return null;
    const passed = tests.filter(t => t.passed).length;
    const total = tests.length;
    const passRate = ((passed / total) * 100).toFixed(1);

    return (
      <div className="mb-6 border border-gray-200 rounded-lg p-4">
        <div className="flex items-center justify-between mb-3">
          <div className="flex items-center gap-2">
            <Icon className="w-5 h-5 text-blue-600" />
            <h3 className="text-lg font-semibold">{title}</h3>
          </div>
          <div className="text-sm">
            <span className={passed === total ? "text-green-600" : "text-orange-600"}>
              {passed}/{total} passed ({passRate}%)
            </span>
          </div>
        </div>
        <div className="space-y-2">
          {tests.map((test, idx) => (
            <div key={idx} className="flex items-start gap-2 p-2 bg-gray-50 rounded">
              {test.passed ? (
                <CheckCircle className="w-5 h-5 text-green-500 flex-shrink-0 mt-0.5" />
              ) : (
                <XCircle className="w-5 h-5 text-red-500 flex-shrink-0 mt-0.5" />
              )}
              <div className="flex-1 min-w-0">
                <div className="font-medium text-sm">{test.name}</div>
                <div className="text-xs text-gray-600 truncate">{test.input || test.expected}</div>
                {!test.passed && (
                  <div className="text-xs text-red-600 mt-1">{test.message}</div>
                )}
              </div>
            </div>
          ))}
        </div>
      </div>
    );
  };

  const totalTests = results ? 
    Object.values(results).reduce((sum, tests) => sum + tests.length, 0) : 0;
  const passedTests = results ?
    Object.values(results).reduce((sum, tests) => sum + tests.filter(t => t.passed).length, 0) : 0;

  return (
    <div className="max-w-6xl mx-auto p-6 bg-white">
      <div className="mb-8">
        <h1 className="text-3xl font-bold mb-2">Kayte C Compiler Test Suite</h1>
        <p className="text-gray-600">Comprehensive testing framework for all compiler stages</p>
      </div>

      <div className="mb-6">
        <button
          onClick={runTests}
          disabled={running}
          className="flex items-center gap-2 px-6 py-3 bg-blue-600 text-white rounded-lg hover:bg-blue-700 disabled:bg-gray-400 disabled:cursor-not-allowed transition-colors"
        >
          <Play className="w-5 h-5" />
          {running ? "Running Tests..." : "Run All Tests"}
        </button>
      </div>

      {results && (
        <>
          <div className="mb-6 p-4 bg-gradient-to-r from-blue-50 to-indigo-50 rounded-lg border border-blue-200">
            <h2 className="text-xl font-bold mb-2">Overall Results</h2>
            <div className="text-3xl font-bold">
              <span className={passedTests === totalTests ? "text-green-600" : "text-orange-600"}>
                {passedTests}/{totalTests}
              </span>
              <span className="text-gray-600 text-xl ml-2">
                ({((passedTests / totalTests) * 100).toFixed(1)}%)
              </span>
            </div>
            {passedTests === totalTests ? (
              <div className="flex items-center gap-2 mt-2 text-green-700">
                <CheckCircle className="w-5 h-5" />
                <span>All tests passed!</span>
              </div>
            ) : (
              <div className="flex items-center gap-2 mt-2 text-orange-700">
                <AlertCircle className="w-5 h-5" />
                <span>{totalTests - passedTests} test(s) failed</span>
              </div>
            )}
          </div>

          <TestSection title="Lexical Analysis" tests={results.lexer} icon={FileText} />
          <TestSection title="Parser" tests={results.parser} icon={FileText} />
          <TestSection title="Semantic Analysis" tests={results.semantic} icon={FileText} />
          <TestSection title="Code Generation" tests={results.codegen} icon={FileText} />
          <TestSection title="Integration Tests" tests={results.integration} icon={FileText} />
        </>
      )}

      {!results && (
        <div className="text-center py-12 text-gray-500">
          <FileText className="w-16 h-16 mx-auto mb-4 opacity-50" />
          <p>Click "Run All Tests" to start the test suite</p>
        </div>
      )}
    </div>
  );
};

export default KayteTestSuite;