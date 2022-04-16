; ModuleID = 'read_write_lock_3.c'
source_filename = "read_write_lock_3.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@l = dso_local global i32 0, align 4
@w = dso_local global i32 0, align 4
@r = dso_local global i32 0, align 4
@x = dso_local global i32 0, align 4
@y = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @writer() #0 {
  br label %1

1:                                                ; preds = %5, %0
  %2 = cmpxchg i32* @l, i32 0, i32 1 seq_cst seq_cst
  %3 = extractvalue { i32, i1 } %2, 1
  %4 = xor i1 %3, true
  br i1 %4, label %5, label %6

5:                                                ; preds = %1
  br label %1

6:                                                ; preds = %1
  %7 = load i32, i32* @w, align 4
  %8 = icmp eq i32 %7, 0
  br i1 %8, label %9, label %13

9:                                                ; preds = %6
  %10 = load i32, i32* @r, align 4
  %11 = icmp eq i32 %10, 0
  br i1 %11, label %12, label %13

12:                                               ; preds = %9
  store i32 1, i32* @w, align 4
  store i32 0, i32* @l, align 4
  store i32 3, i32* @x, align 4
  store i32 0, i32* @w, align 4
  br label %14

13:                                               ; preds = %9, %6
  store i32 0, i32* @l, align 4
  br label %14

14:                                               ; preds = %13, %12
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @reader() #0 {
  br label %1

1:                                                ; preds = %5, %0
  %2 = cmpxchg i32* @l, i32 0, i32 1 seq_cst seq_cst
  %3 = extractvalue { i32, i1 } %2, 1
  %4 = xor i1 %3, true
  br i1 %4, label %5, label %6

5:                                                ; preds = %1
  br label %1

6:                                                ; preds = %1
  %7 = load i32, i32* @w, align 4
  %8 = icmp eq i32 %7, 0
  br i1 %8, label %9, label %20

9:                                                ; preds = %6
  %10 = load i32, i32* @r, align 4
  %11 = add nsw i32 %10, 1
  store i32 %11, i32* @r, align 4
  store i32 0, i32* @l, align 4
  %12 = load i32, i32* @x, align 4
  store i32 %12, i32* @y, align 4
  %13 = load i32, i32* @y, align 4
  %14 = load i32, i32* @x, align 4
  %15 = icmp eq i32 %13, %14
  br i1 %15, label %17, label %16

16:                                               ; preds = %9
  call void @__ASSERT()
  br label %17

17:                                               ; preds = %16, %9
  %18 = load i32, i32* @r, align 4
  %19 = sub nsw i32 %18, 1
  store i32 %19, i32* @r, align 4
  br label %21

20:                                               ; preds = %6
  store i32 0, i32* @l, align 4
  br label %21

21:                                               ; preds = %20, %17
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @writer2() #0 {
  br label %1

1:                                                ; preds = %5, %0
  %2 = cmpxchg i32* @l, i32 0, i32 1 seq_cst seq_cst
  %3 = extractvalue { i32, i1 } %2, 1
  %4 = xor i1 %3, true
  br i1 %4, label %5, label %6

5:                                                ; preds = %1
  br label %1

6:                                                ; preds = %1
  %7 = load i32, i32* @w, align 4
  %8 = icmp eq i32 %7, 0
  br i1 %8, label %9, label %13

9:                                                ; preds = %6
  %10 = load i32, i32* @r, align 4
  %11 = icmp eq i32 %10, 0
  br i1 %11, label %12, label %13

12:                                               ; preds = %9
  store i32 1, i32* @w, align 4
  store i32 0, i32* @l, align 4
  store i32 3, i32* @x, align 4
  store i32 0, i32* @w, align 4
  br label %14

13:                                               ; preds = %9, %6
  store i32 0, i32* @l, align 4
  br label %14

14:                                               ; preds = %13, %12
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
