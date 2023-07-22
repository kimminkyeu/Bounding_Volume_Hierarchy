//
// Created by USER on 2023-07-20.
//

#ifndef SCOOP_CONFIG_H
#define SCOOP_CONFIG_H

#ifndef PROJECT_ROOT_DIR // cmake에서 반드시 설정해주어야 함.
# if NOT_DEFINED_THEN_COMPILATION_FAILS()
// function-macro는 undefine시 컴파일 에러 발생함을 이용.
# endif
#endif


#endif //SCOOP_CONFIG_H
