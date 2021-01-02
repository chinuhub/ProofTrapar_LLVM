; ModuleID = 'lamport.c'
source_filename = "lamport.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@b1 = common dso_local global i32 0, align 4
@x = common dso_local global i32 0, align 4
@y = common dso_local global i32 0, align 4
@b2 = common dso_local global i32 0, align 4
@X = common dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  br label %4

4:                                                ; preds = %0, %15, %38
  store i32 1, i32* @b1, align 4
  store i32 1, i32* @x, align 4
  %5 = load i32, i32* @y, align 4
  store i32 %5, i32* %1, align 4
  %6 = load i32, i32* %1, align 4
  %7 = icmp ne i32 %6, 0
  br i1 %7, label %8, label %16

8:                                                ; preds = %4
  store i32 0, i32* @b1, align 4
  %9 = load i32, i32* @y, align 4
  store i32 %9, i32* %1, align 4
  br label %10

10:                                               ; preds = %13, %8
  %11 = load i32, i32* %1, align 4
  %12 = icmp ne i32 %11, 0
  br i1 %12, label %13, label %15

13:                                               ; preds = %10
  %14 = load i32, i32* @y, align 4
  store i32 %14, i32* %1, align 4
  br label %10

15:                                               ; preds = %10
  br label %4

16:                                               ; preds = %4
  store i32 1, i32* @y, align 4
  %17 = load i32, i32* @x, align 4
  store i32 %17, i32* %2, align 4
  %18 = load i32, i32* %2, align 4
  %19 = icmp ne i32 %18, 1
  br i1 %19, label %20, label %40

20:                                               ; preds = %16
  store i32 0, i32* @b1, align 4
  %21 = load i32, i32* @b2, align 4
  store i32 %21, i32* %3, align 4
  br label %22

22:                                               ; preds = %25, %20
  %23 = load i32, i32* %3, align 4
  %24 = icmp sge i32 %23, 1
  br i1 %24, label %25, label %27

25:                                               ; preds = %22
  %26 = load i32, i32* @b2, align 4
  store i32 %26, i32* %3, align 4
  br label %22

27:                                               ; preds = %22
  %28 = load i32, i32* @y, align 4
  store i32 %28, i32* %1, align 4
  %29 = load i32, i32* %1, align 4
  %30 = icmp ne i32 %29, 1
  br i1 %30, label %31, label %39

31:                                               ; preds = %27
  %32 = load i32, i32* @y, align 4
  store i32 %32, i32* %1, align 4
  br label %33

33:                                               ; preds = %36, %31
  %34 = load i32, i32* %1, align 4
  %35 = icmp ne i32 %34, 0
  br i1 %35, label %36, label %38

36:                                               ; preds = %33
  %37 = load i32, i32* @y, align 4
  store i32 %37, i32* %1, align 4
  br label %33

38:                                               ; preds = %33
  br label %4

39:                                               ; preds = %27
  br label %40

40:                                               ; preds = %39, %16
  br label %41

41:                                               ; preds = %40
  store i32 0, i32* @X, align 4
  %42 = load i32, i32* @X, align 4
  %43 = icmp sle i32 %42, 0
  br i1 %43, label %45, label %44

44:                                               ; preds = %41
  call void @__ASSERT()
  br label %45

45:                                               ; preds = %44, %41
  store i32 0, i32* @y, align 4
  store i32 0, i32* @b1, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  br label %4

4:                                                ; preds = %0, %15, %38
  store i32 1, i32* @b2, align 4
  store i32 2, i32* @x, align 4
  %5 = load i32, i32* @y, align 4
  store i32 %5, i32* %1, align 4
  %6 = load i32, i32* %1, align 4
  %7 = icmp ne i32 %6, 0
  br i1 %7, label %8, label %16

8:                                                ; preds = %4
  store i32 0, i32* @b2, align 4
  %9 = load i32, i32* @y, align 4
  store i32 %9, i32* %1, align 4
  br label %10

10:                                               ; preds = %13, %8
  %11 = load i32, i32* %1, align 4
  %12 = icmp ne i32 %11, 0
  br i1 %12, label %13, label %15

13:                                               ; preds = %10
  %14 = load i32, i32* @y, align 4
  store i32 %14, i32* %1, align 4
  br label %10

15:                                               ; preds = %10
  br label %4

16:                                               ; preds = %4
  store i32 2, i32* @y, align 4
  %17 = load i32, i32* @x, align 4
  store i32 %17, i32* %2, align 4
  %18 = load i32, i32* %2, align 4
  %19 = icmp ne i32 %18, 2
  br i1 %19, label %20, label %40

20:                                               ; preds = %16
  store i32 0, i32* @b2, align 4
  %21 = load i32, i32* @b1, align 4
  store i32 %21, i32* %3, align 4
  br label %22

22:                                               ; preds = %25, %20
  %23 = load i32, i32* %3, align 4
  %24 = icmp sge i32 %23, 1
  br i1 %24, label %25, label %27

25:                                               ; preds = %22
  %26 = load i32, i32* @b1, align 4
  store i32 %26, i32* %3, align 4
  br label %22

27:                                               ; preds = %22
  %28 = load i32, i32* @y, align 4
  store i32 %28, i32* %1, align 4
  %29 = load i32, i32* %1, align 4
  %30 = icmp ne i32 %29, 2
  br i1 %30, label %31, label %39

31:                                               ; preds = %27
  %32 = load i32, i32* @y, align 4
  store i32 %32, i32* %1, align 4
  br label %33

33:                                               ; preds = %36, %31
  %34 = load i32, i32* %1, align 4
  %35 = icmp ne i32 %34, 0
  br i1 %35, label %36, label %38

36:                                               ; preds = %33
  %37 = load i32, i32* @y, align 4
  store i32 %37, i32* %1, align 4
  br label %33

38:                                               ; preds = %33
  br label %4

39:                                               ; preds = %27
  br label %40

40:                                               ; preds = %39, %16
  br label %41

41:                                               ; preds = %40
  store i32 1, i32* @X, align 4
  %42 = load i32, i32* @X, align 4
  %43 = icmp sge i32 %42, 1
  br i1 %43, label %45, label %44

44:                                               ; preds = %41
  call void @__ASSERT()
  br label %45

45:                                               ; preds = %44, %41
  store i32 0, i32* @y, align 4
  store i32 0, i32* @b2, align 4
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
