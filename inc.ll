; ModuleID = 'inc.c'
source_filename = "inc.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@lock = dso_local global i32 0, align 4
@value = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  br label %2

2:                                                ; preds = %6, %0
  %3 = cmpxchg i32* @lock, i32 0, i32 1 seq_cst seq_cst
  %4 = extractvalue { i32, i1 } %3, 1
  %5 = xor i1 %4, true
  br i1 %5, label %6, label %7

6:                                                ; preds = %2
  br label %2

7:                                                ; preds = %2
  %8 = load i32, i32* @value, align 4
  %9 = icmp eq i32 %8, -1
  br i1 %9, label %10, label %11

10:                                               ; preds = %7
  store i32 0, i32* @lock, align 4
  br label %20

11:                                               ; preds = %7
  %12 = load i32, i32* @value, align 4
  store i32 %12, i32* %1, align 4
  %13 = load i32, i32* %1, align 4
  %14 = add i32 %13, 1
  store i32 %14, i32* @value, align 4
  store i32 0, i32* @lock, align 4
  %15 = load i32, i32* @value, align 4
  %16 = load i32, i32* %1, align 4
  %17 = icmp ugt i32 %15, %16
  br i1 %17, label %19, label %18

18:                                               ; preds = %11
  call void @__ASSERT()
  br label %19

19:                                               ; preds = %18, %11
  br label %20

20:                                               ; preds = %19, %10
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  br label %2

2:                                                ; preds = %6, %0
  %3 = cmpxchg i32* @lock, i32 0, i32 1 seq_cst seq_cst
  %4 = extractvalue { i32, i1 } %3, 1
  %5 = xor i1 %4, true
  br i1 %5, label %6, label %7

6:                                                ; preds = %2
  br label %2

7:                                                ; preds = %2
  %8 = load i32, i32* @value, align 4
  %9 = icmp eq i32 %8, -1
  br i1 %9, label %10, label %11

10:                                               ; preds = %7
  store i32 0, i32* @lock, align 4
  br label %20

11:                                               ; preds = %7
  %12 = load i32, i32* @value, align 4
  store i32 %12, i32* %1, align 4
  %13 = load i32, i32* %1, align 4
  %14 = add i32 %13, 1
  store i32 %14, i32* @value, align 4
  store i32 0, i32* @lock, align 4
  %15 = load i32, i32* @value, align 4
  %16 = load i32, i32* %1, align 4
  %17 = icmp ugt i32 %15, %16
  br i1 %17, label %19, label %18

18:                                               ; preds = %11
  call void @__ASSERT()
  br label %19

19:                                               ; preds = %18, %11
  br label %20

20:                                               ; preds = %19, %10
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
