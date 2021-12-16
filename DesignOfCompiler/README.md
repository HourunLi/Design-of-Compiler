- 测试环境：
  - UBANTU
  - y可执行文件是在linux环境下生成的，window不能直接运行
- 源代码：
  - lexical_analysis.l 词法分析
  - syntax_analysis.y 和node.h 语法分析
  - semantic_analysis.y 语义分析
  - interpret.cpp翻译代码文件
  - ==其中语法分析相关文件应该不会用到，我也不保证修改之后还能不能用。要看语法分析的代码为什么不找上次作业？==
  - 主要就是semantic_analysis.y和lexical_analysis.l两个文件构成语义分析大作业的源代码。
- 测试文件
  - for.pl0 测试for语句
  - test.pl0 有三层嵌套的复杂pl0代码
- 输出文件：
  - test.pcode是test.pl0的pcode代码输出文件
  - for.pcode是for.pl0的pcode代码输出文件
  - y是生成pcode代码的可执行文件，**若要直接使用，需要在linux环境下使用；否则自行重新编译。**
  - I是解释pcode代码的可执行文件，**若要直接使用，需要在linux环境下使用；否则自行重新编译。**

- 编译命令

  ```shell
  g++ y.tab.c -o y
  yacc -d syntax_analysis.y
  flex -o LA.cpp lexical_analysis.l
  g++ interpret.cpp -o I
  ```

- 运行命令

  ```
  ./y test.pl0 test.pcode
  ./I test.pcode
  ```

  