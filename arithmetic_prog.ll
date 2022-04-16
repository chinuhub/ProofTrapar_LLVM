; ModuleID = 'arithmetic_prog.c'
source_filename = "arithmetic_prog.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@m = dso_local global i32 0, align 4
@cond1 = dso_local global i32 0, align 4
@lock1 = dso_local global i32 0, align 4
@cond2 = dso_local global i32 0, align 4
@lock2 = dso_local global i32 0, align 4
@num = dso_local global i32 0, align 4
@total = dso_local global i32 0, align 4
@checking = dso_local global i32 0, align 4
@flag = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thread1() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  br label %3

3:                                                ; preds = %28, %0
  %4 = load i32, i32* %1, align 4
  %5 = icmp slt i32 %4, 4
  br i1 %5, label %6, label %33

6:                                                ; preds = %3
  br label %7

7:                                                ; preds = %11, %6
  %8 = cmpxchg i32* @m, i32 0, i32 1 seq_cst seq_cst
  %9 = extractvalue { i32, i1 } %8, 1
  %10 = xor i1 %9, true
  br i1 %10, label %11, label %12

11:                                               ; preds = %7
  br label %7

12:                                               ; preds = %7
  br label %13

13:                                               ; preds = %27, %12
  %14 = load i32, i32* @num, align 4
  %15 = icmp sgt i32 %14, 0
  br i1 %15, label %16, label %28

16:                                               ; preds = %13
  br label %17

17:                                               ; preds = %21, %16
  %18 = cmpxchg i32* @lock1, i32 0, i32 1 seq_cst seq_cst
  %19 = extractvalue { i32, i1 } %18, 1
  %20 = xor i1 %19, true
  br i1 %20, label %21, label %22

21:                                               ; preds = %17
  br label %17

22:                                               ; preds = %17
  store i32 0, i32* @m, align 4
  br label %23

23:                                               ; preds = %26, %22
  %24 = load i32, i32* @cond1, align 4
  %25 = icmp ne i32 %24, 1
  br i1 %25, label %26, label %27

26:                                               ; preds = %23
  br label %23

27:                                               ; preds = %23
  store i32 0, i32* @lock1, align 4
  br label %13

28:                                               ; preds = %13
  %29 = load i32, i32* @num, align 4
  %30 = add nsw i32 %29, 1
  store i32 %30, i32* @num, align 4
  store i32 0, i32* @m, align 4
  store i32 1, i32* @cond2, align 4
  %31 = load i32, i32* %1, align 4
  %32 = add nsw i32 %31, 1
  store i32 %32, i32* %1, align 4
  br label %3

33:                                               ; preds = %3
  %34 = load i32, i32* @checking, align 4
  store i32 %34, i32* %2, align 4
  store i32 1, i32* @checking, align 4
  br label %35

35:                                               ; preds = %39, %33
  %36 = cmpxchg i32* @m, i32 0, i32 1 seq_cst seq_cst
  %37 = extractvalue { i32, i1 } %36, 1
  %38 = xor i1 %37, true
  br i1 %38, label %39, label %40

39:                                               ; preds = %35
  br label %35

40:                                               ; preds = %35
  %41 = load i32, i32* %2, align 4
  %42 = icmp eq i32 %41, 2
  br i1 %42, label %43, label %52

43:                                               ; preds = %40
  %44 = load i32, i32* @flag, align 4
  %45 = icmp ne i32 %44, 0
  br i1 %45, label %46, label %51

46:                                               ; preds = %43
  %47 = load i32, i32* @total, align 4
  %48 = icmp eq i32 %47, 10
  br i1 %48, label %50, label %49

49:                                               ; preds = %46
  call void @__ASSERT()
  br label %50

50:                                               ; preds = %49, %46
  br label %51

51:                                               ; preds = %50, %43
  br label %52

52:                                               ; preds = %51, %40
  store i32 0, i32* @m, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thread2() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  br label %3

3:                                                ; preds = %28, %0
  %4 = load i32, i32* %1, align 4
  %5 = icmp slt i32 %4, 4
  br i1 %5, label %6, label %36

6:                                                ; preds = %3
  br label %7

7:                                                ; preds = %11, %6
  %8 = cmpxchg i32* @m, i32 0, i32 1 seq_cst seq_cst
  %9 = extractvalue { i32, i1 } %8, 1
  %10 = xor i1 %9, true
  br i1 %10, label %11, label %12

11:                                               ; preds = %7
  br label %7

12:                                               ; preds = %7
  br label %13

13:                                               ; preds = %27, %12
  %14 = load i32, i32* @num, align 4
  %15 = icmp eq i32 %14, 0
  br i1 %15, label %16, label %28

16:                                               ; preds = %13
  br label %17

17:                                               ; preds = %21, %16
  %18 = cmpxchg i32* @lock2, i32 0, i32 1 seq_cst seq_cst
  %19 = extractvalue { i32, i1 } %18, 1
  %20 = xor i1 %19, true
  br i1 %20, label %21, label %22

21:                                               ; preds = %17
  br label %17

22:                                               ; preds = %17
  store i32 0, i32* @m, align 4
  br label %23

23:                                               ; preds = %26, %22
  %24 = load i32, i32* @cond2, align 4
  %25 = icmp ne i32 %24, 1
  br i1 %25, label %26, label %27

26:                                               ; preds = %23
  br label %23

27:                                               ; preds = %23
  store i32 0, i32* @lock2, align 4
  br label %13

28:                                               ; preds = %13
  %29 = load i32, i32* @total, align 4
  %30 = load i32, i32* %1, align 4
  %31 = add nsw i32 %29, %30
  store i32 %31, i32* @total, align 4
  %32 = load i32, i32* @num, align 4
  %33 = add nsw i32 %32, -1
  store i32 %33, i32* @num, align 4
  store i32 0, i32* @m, align 4
  store i32 1, i32* @cond1, align 4
  %34 = load i32, i32* %1, align 4
  %35 = add nsw i32 %34, 1
  store i32 %35, i32* %1, align 4
  br label %3

36:                                               ; preds = %3
  %37 = load i32, i32* @total, align 4
  %38 = load i32, i32* %1, align 4
  %39 = add nsw i32 %37, %38
  store i32 %39, i32* @total, align 4
  store i32 1, i32* @flag, align 4
  %40 = load i32, i32* @checking, align 4
  store i32 %40, i32* %2, align 4
  store i32 2, i32* @checking, align 4
  br label %41

41:                                               ; preds = %45, %36
  %42 = cmpxchg i32* @m, i32 0, i32 1 seq_cst seq_cst
  %43 = extractvalue { i32, i1 } %42, 1
  %44 = xor i1 %43, true
  br i1 %44, label %45, label %46

45:                                               ; preds = %41
  br label %41

46:                                               ; preds = %41
  %47 = load i32, i32* %2, align 4
  %48 = icmp eq i32 %47, 1
  br i1 %48, label %49, label %58

49:                                               ; preds = %46
  %50 = load i32, i32* @flag, align 4
  %51 = icmp ne i32 %50, 0
  br i1 %51, label %52, label %57

52:                                               ; preds = %49
  %53 = load i32, i32* @total, align 4
  %54 = icmp eq i32 %53, 10
  br i1 %54, label %56, label %55

55:                                               ; preds = %52
  call void @__ASSERT()
  br label %56

56:                                               ; preds = %55, %52
  br label %57

57:                                               ; preds = %56, %49
  br label %58

58:                                               ; preds = %57, %46
  store i32 0, i32* @m, align 4
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
